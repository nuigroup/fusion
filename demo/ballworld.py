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

class BallworldWidget(Button):
    def on_mouse_down(self, x, y, button, modifiers):
        print x + "," + y


class TestApp(App):
    
    def build(self):
        def utterance_begin(*l):
            print "start"
            r = requests.get('http://127.0.0.1:7500/utterance/begin')
            print r
            r = requests.get('http://127.0.0.1:7500/utterance/speech/begin')
            print r
        def utterance_end(*l):
            time.sleep(0.25)
            print "end"
            r = requests.get('http://127.0.0.1:7500/utterance/speech/end')
            print "gesture"
            r = requests.get('http://127.0.0.1:7500/utterance/gesture/data')
            print r
            r = requests.get('http://127.0.0.1:7500/utterance/end')
            print r
        # return a Button() as a root widget
        btn = Button(text='utterance')
        btn.bind(on_press=utterance_begin)
        btn.bind(on_release=utterance_end)
        return btn
    
if __name__ == '__main__':
    TestApp().run()

