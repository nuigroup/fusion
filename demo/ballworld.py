'''
    Application example using build() + return
    ==========================================
    
    An application can be build if you return a widget on build(), or if you set
    self.root.
    '''
import requests

import time

import kivy
kivy.require('1.0.7')

from kivy.app import App
from kivy.uix.button import Button
from kivy.core.window import Window
from kivy.config import Config
from os.path import join, dirname
from kivy.properties import StringProperty
from glob import glob
from kivy.logger import Logger
from kivy.uix.widget import Widget
from kivy.graphics import Color, Ellipse, Line

class BallWorld(App):
    
    def build(self):
        
        def utterance_begin(*l):
            print "start"
            r = requests.get('http://127.0.0.1:7500/utterance/begin')
            print r.content
            r = requests.get('http://127.0.0.1:7500/utterance/speech/begin')
            print r.content
        
        def utterance_end(*l):
            time.sleep(0.25)
            print "end"
            r = requests.get('http://127.0.0.1:7500/utterance/speech/end')
            #print "gesture"
            #r = requests.get('http://127.0.0.1:7500/utterance/gesture/data')
            print r.content
            r = requests.get('http://127.0.0.1:7500/utterance/end')
            print r.content
        btn = Button(text='utterance')
        btn.bind(on_press=utterance_begin)
        btn.bind(on_release=utterance_end)
        return btn
                    
if __name__ == '__main__':
    Config.set('input', 'default', 'mouse')
    Config.write()
    BallWorld().run()

