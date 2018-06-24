#!/usr/bin/env python3

import tkinter as tk
import socket as so

class Arduino(object):
    def __init__(self, window, host, port, on_received):
        self.window= window
        self.on_received= on_received

        # Open socket and connect to the Arduino
        self.socket= so.socket()
        self.socket.connect((host, port))
        self.socket.setblocking(False)

        self.rd_buff= bytes()

        self._periodic_socket_check()

    def send_command(self, command):
        'Send a message to the Arduino'

        self.socket.send(command.encode('utf-8') + b'\n')

    def close(self):
        'Cleanly close the connection'

        self.socket.close()
        self.window.after_cancel(self.after_event)

    def _periodic_socket_check(self):
        try:
            msg= self.socket.recv(1024)

            if not msg:
                raise(IOError('Connection closed'))

            self.rd_buff+= msg

        except so.error:
            # In non-blocking mode an exception is thrown
            # whenever no data is available.
            # Which error is OS-dependant so we have to catch
            # the generic socket.error exception.

            pass

        while b'\n' in self.rd_buff:
            line, self.rd_buff= self.rd_buff.split(b'\n')

            line= line.decode('utf-8').strip()
            self.on_received(line)

        # Tkinter should call the function again
        # in 100ms
        self.after_event= self.window.after(
            100, self._periodic_socket_check
        )

class ResetButton(object):
    '''A button that controls one LED connected to an Arduino
    while providing visual feedback of the current button state
    '''
    
    def __init__(self, window, arduino):
        self.button= tk.Button(
            window,
            text='Reset', fg="red",
            command= self.on_pressed
        )

        self.button.pack()

        self.arduino= arduino

        self.set_state(False)

    def set_state(self, state):
        if state:
            self.arduino.send_command('on')
            self.state= True

        else:
            self.arduino.send_command('off')
            self.state= False

    def on_pressed(self):
        self.set_state(not self.state)

class BriefkastenWindow(object):
    def __init__(self):
        # Setup the empty window
        self.setup_window()

        host= input('Hostname: ')
        port= input('Port: ')

        self.arduino= Arduino(
            self.window,
            host, int(port),
            self.on_received
        )

        self.setup_content()

    def on_received(self, line):
        self.btn_label_var.set('Briefeinwürfe {}'.format(line))
    

    def setup_window(self):
        self.window= tk.Tk()
        self.window.geometry('200x200')
        self.window.configure(background='cyan')
        self.window.title('Mail Overview')
        self.window.protocol("WM_DELETE_WINDOW", self.on_close)

    def on_close(self):
        self.arduino.close()
        self.window.destroy()

    def setup_content(self):
        self.btn_label_var= tk.StringVar(self.window)
        self.btn_label_var.set('Briefeinwürfe: 0')

        self.btn_label = tk.Label(
            self.window,
            textvariable= self.btn_label_var
        )

        self.btn_label.pack()

        self.btn= ResetButton(self.window, self.arduino)

    def run(self):
        'Execute the tkinter mainloop to display the Light center window'

        self.window.mainloop()

if __name__ == '__main__':
    window= BriefkastenWindow()
    window.run()
