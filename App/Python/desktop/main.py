# IoBot App v1.0 Copyright 2015 Zygmunt Wojcik
# http://iobot.info/
# GNU GPL version 3 License - http://www.gnu.org/licenses/

ver = '1.0'

import socket

import serial
import serial.tools.list_ports

import kivy
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import ObjectProperty, StringProperty, ListProperty, BooleanProperty
from kivy.uix.popup import Popup

from kivy.uix.settings import SettingsWithTabbedPanel

from kivy.config import Config


class RootWidget(BoxLayout):
    connection_button = ObjectProperty()
    connected = BooleanProperty(False)
    serial_port = StringProperty()
    ports = []
    ser = ObjectProperty()
    error_popup = ObjectProperty()
    status_label = ObjectProperty()
    
    def connect_button_action(self):
        if self.serial_port is not None:
            if self.connected is False:
                self.serial_connect()
                
            elif self.connected is True:
                self.serial_disconnect()
                
    def serial_connect(self):
        try:
            self.ser = serial.Serial('{}'.format(self.serial_port), 115200)
            self.connected = True
            
        except (OSError, serial.serialutil.SerialException): 
            if self.error_popup is None:
                self.error_popup = CustomPopup()
                
            if self.serial_port == 'Port...':
                self.error_popup.open_error_popup('Check serial port settings')
                
            else:
                self.error_popup.open_error_popup('Can not connect with this port')
            
    def serial_disconnect(self):
        self.ser.close()
        self.connected = False
        
    def on_connected(self, *args):
        self.connection_button.text = 'Disconnect' if self.connected is True else 'Connect'
        IoBot.connected = self.connected
        IoBot.ser = self.ser
    
    def get_ser_ports(self):
        data = list(serial.tools.list_ports.comports())
        self.ports = []
        
        for port in data:
            self.ports.append(port[0])
            
        return self.ports

    def connection_method_action(self):
        if self.connected is True:
            self.serial_disconnect()
        status_template = '{connection} Connection: {address}'
        
        if self.children[0].connection_method == 'LAN':
            self.status_label.text = status_template.format(connection = 'LAN',
                                                            address = self.children[0].ip)
        elif self.children[0].connection_method == 'Serial':
            self.status_label.text = status_template.format(connection = "Serial",
                                                            address = self.serial_port)
            
    def action_previous_action(self):
        if self.error_popup is None:
            self.error_popup = CustomPopup()
            
        self.error_popup.open_error_popup('IoBot v{} - Copyright 2015 Zygmunt Wojcik www.iobot.info'.format(ver))
        

class IoBot(BoxLayout):
    ip = StringProperty('')
    connection_method = []
    connected = BooleanProperty(False)
    ser = ObjectProperty()
    error_popup = ObjectProperty()
        
    def send_command(self, _command):
        command = 'aIB' + _command      
        while len(command) < 12:
            command +='-'
            
        if self.connection_method == 'LAN':
            self.send_udp(command)  
             
        elif self.connection_method == 'Serial' and self.connected is True:
            self.send_serial(command)
            
        elif self.connection_method == 'Serial' and self.parent.serial_port == 'Port...':
            if self.error_popup is None:
                self.error_popup = CustomPopup()
            title = 'Check serial port settings'
            self.error_popup.open_error_popup(title)
            
        elif self.connection_method == 'Serial' and self.parent.serial_port is not 'Port...':
            if self.error_popup is None:
                self.error_popup = CustomPopup()
            title = 'Remote device is not connected'
            self.error_popup.open_error_popup(title)
            
        else:
            if self.error_popup is None:
                self.error_popup = CustomPopup()
            title = 'Please setup your device connection'
            self.error_popup.open_error_popup(title)
            
    def send_udp(self, command):
        if self.error_popup is None:
            self.error_popup = CustomPopup()
            
        if self.error_popup.opened is False:
            UDP_IP = self.ip
            UDP_PORT = 8888
            print "UDP target IP:", UDP_IP
            print "message:", command
            
            try:
                sock = socket.socket(socket.AF_INET, # Internet
                			        socket.SOCK_DGRAM) # UDP
                sock.sendto(command, (UDP_IP, UDP_PORT))
                
            except (UnicodeError, socket.error):
                title = 'Please check IP address settings'
                self.error_popup.open_error_popup(title)         
                
    def send_serial(self, command):
        if self.error_popup is None:
            self.error_popup = CustomPopup()
            
        if self.error_popup.opened is False:
            print "Serial message:", command
            
            try:
                self.ser.write(command)
                
            except serial.serialutil.SerialException:
                if self.connected is True:
                    self.parent.serial_disconnect()
                title = 'There is something wrong with serial connection'
                self.error_popup.open_error_popup(title)
                
                
class CustomPopup(Popup):
    opened = False
    popup_button = ObjectProperty()
        
    def on_dismiss(self):
        self.opened = False
        
    def open_error_popup(self, title):
        if self.opened is False:
            self.title = title
            self.popup_button.text = 'Click me to dismiss'
            self.open()
            self.opened = True
        
        
class IoBotApp(App):
    use_kivy_settings = False
    settings_cls = SettingsWithTabbedPanel
    icon = 'iobot_icon_256.ico'
    
    def on_start(self):
        connection_method = self.config.getdefault("General", "connection_method", "")
        IoBot.connection_method = connection_method
        
        ip = self.config.getdefault("General", "ip", "")
        IoBot.ip = ip
        
        serial_port = self.config.getdefault("General", "serial_port", "")
        RootWidget.serial_port = serial_port
        
        self.root.connection_button.disabled = False if connection_method == 'Serial' else True
        
        status_label = self.root.status_label
        status_template = '{connection} Connection: {address}'
        
        if connection_method == 'LAN':
            status_label.text = status_template.format(connection = connection_method,
                                                        address = ip)
        elif connection_method == 'Serial':
            status_label.text = status_template.format(connection = connection_method,
                                                        address = serial_port)
        
    def build_config(self, config):
        config.setdefaults('General', {'connection_method': 'LAN',
                                        'serial_port': 'Serial Port...',
                                        'ip': '192.168.1.177'})
        Config.set('kivy', 'window_icon', 'iobot_icon_256.ico')
                                        
    def build_settings(self, settings):
        root_widget = self.root
        ports = str(root_widget.get_ser_ports())
        ports_json = ports.replace('\'', '\"')
        json_string = """
        [
            {
                "type": "options",
                "title": "Connection Method:",
                "desc": "Choose Connection Method, 'LAN' or 'Serial'",
                "section": "General",
                "key": "connection_method",
                "options": [ "LAN", "Serial"]
            },
            {
                "type": "options",
                "title": "Serial Port:",
                "desc": "Choose Serial Port if 'Connection Method' is 'Serial'",
                "section": "General",
                "key": "serial_port",
                "options": """ + ports_json + """
            },
            {
                "type": "string",
                "title": "IP Address",
                "desc": "Choose IP Address if 'Connection Method' is 'LAN'",
                "section": "General",
                "key": "ip"  
            }  
        ]
        """
        settings.add_json_panel('Settings', self.config, data = json_string)
                
    def close_settings(self, *largs):
            win = self._app_window
            settings = self._app_settings
            
            if win is None or settings is None:
                return
                
            if settings in win.children:
                win.remove_widget(settings)
                self.destroy_settings()
                return True
                
            return False
            
    def on_config_change(self, config, section, key, value):
        if config is self.config and key == "connection_method":
            self.root.children[0].connection_method = value     # IoBot
            self.root.connection_button.disabled = False if value == 'Serial' else True
            
        elif config is self.config and key == "serial_port":
            self.root.serial_port = value      # RootWidget
            
        elif config is self.config and key == "ip":
            self.root.children[0].ip = value    # IoBot
            
        self.root.connection_method_action()    # RootWidget
        

if __name__ == '__main__':
    IoBotApp().run()