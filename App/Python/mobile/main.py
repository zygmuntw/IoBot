# IoBot App v1.0 Copyright 2015 Zygmunt Wojcik
# http://iobot.info/
# GNU GPL version 3 License - http://www.gnu.org/licenses/

ver = '1.0'

import socket

import kivy
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import ObjectProperty, StringProperty, ListProperty, BooleanProperty
from kivy.uix.popup import Popup

from kivy.uix.settings import SettingsWithTabbedPanel


class RootWidget(BoxLayout):
    error_popup = ObjectProperty()
    status_label = ObjectProperty()
    
    def connection_method_action(self):
        status_template = '{connection} Connection: {address}'
        
        self.status_label.text = status_template.format(connection = 'LAN',
                                                        address = self.children[0].ip)
            
    def action_previous_action(self):
        if self.error_popup is None:
            self.error_popup = CustomPopup()
            
        self.error_popup.open_error_popup('IoBot v{} - Copyright 2015 Zygmunt Wojcik www.iobot.info'.format(ver))
        

class IoBot(BoxLayout):
    ip = StringProperty('')
    error_popup = ObjectProperty()
        
    def send_command(self, _command):
        command = 'aIB' + _command      
        while len(command) < 12:
            command +='-'
            
        self.send_udp(command)  
                         
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
    icon = 'iobot_icon_512.png'
    
    def on_start(self):
        
        ip = self.config.getdefault("General", "ip", "")
        IoBot.ip = ip
                
        status_label = self.root.status_label
        status_template = '{connection} Connection: {address}'
        
        status_label.text = status_template.format(connection = 'LAN',
                                                    address = ip)
        
    def build_config(self, config):
        config.setdefaults('General', {'ip': '192.168.1.177'})
                                        
    def build_settings(self, settings):
        root_widget = self.root

        json_string = """
        [
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
                            
    def on_config_change(self, config, section, key, value):            
        if config is self.config and key == "ip":
            self.root.children[0].ip = value    # IoBot
            
        self.root.connection_method_action()    # RootWidget
        
    def on_pause(self):
        return True
        

if __name__ == '__main__':
    IoBotApp().run()