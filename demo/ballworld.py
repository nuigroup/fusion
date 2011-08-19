'''
    Application example using build() + return
    ==========================================
    
    An application can be build if you return a widget on build(), or if you set
    self.root.
    '''
import requests

import time

import json

import kivy
kivy.require('1.0.7')

from kivy.app import App
from kivy.uix.button import Button
from kivy.core.window import Window
from kivy.config import Config
from os.path import join, dirname
from kivy.properties import StringProperty, NumericProperty, ListProperty 
from glob import glob
from kivy.logger import Logger
from kivy.uix.widget import Widget
from kivy.graphics import Color, Ellipse, Line

utteranceStarted = False
ballCounter = 0
gestureBuffer = []

class Ball(Widget):
    id = NumericProperty(0)
    x = NumericProperty(25)
    y = NumericProperty(25)
    color = ListProperty([1, 1, 1])
    width = NumericProperty(50)
    height = NumericProperty(50)

    def on_touch_down(self, touch):
        global gestureBuffer
        if self.collide_point(*touch.pos) and utteranceStarted:
            print "Ball: %d" % self.id
            gestureBuffer.append({"type" : "ball", "val" : self.id})
            return True

class BallWorld(App):
    
    def utterance_get(self, *args):
        global ballCounter
        print "get"
        r = requests.get('http://127.0.0.1:7500/utterance/get')
        if(r):
            print r.content
            tree = json.loads(r.content)
            colorIndex = {"red" : [1, 0, 0], "green" : [0, 1, 0], "blue" : [0, 0, 1], "white" : [1, 1, 1], "yellow" : [1, 1, 0], "purple" : [1, 0, 1], "orange" : [1, 0.5, 0]}
            if tree.has_key("val"):
                if tree["val"] == "create":
                    coords = tree["children"][1]["val"].split(',')
                    print coords
                    self.root.add_widget(Ball(center_x = int(coords[0]), center_y = int(coords[1]), color=colorIndex[tree["children"][0]["val"]], id=ballCounter))
                    ballCounter += 1
                elif tree["val"] == "destroy":
                    for attrs in tree["children"]:
                        if attrs.has_key("val") and attrs.has_key("type"):
                            if attrs["type"] == "ball":
                                for child in self.root.children:
                                    if hasattr(child, "id"):
                                        if child.id == int(tree["children"][0]["val"]):
                                            print "found ball by id"
                                            print child.id
                                            self.root.remove_widget(child)
                            elif attrs["type"] == "color":
                                for child in self.root.children:
                                    if hasattr(child, "color"):
                                        if child.color == colorIndex[tree["children"][0]["val"]]:
                                            print "found ball by color"
                                            print child.id
                                            print child.color
                                            self.root.remove_widget(child)
                elif tree["val"] == "color":
                    if tree["children"][0]["type"] == "ball":
                        for child in self.root.children:
                            if hasattr(child, "id"):
                                if child.id == int(tree["children"][0]["val"]):
                                    print "found ball by id"
                                    print child.id
                                    print child.color
                                    child.color = colorIndex[tree["children"][1]["val"]]
                    elif tree["children"][0]["type"] == "color":
                        for child in self.root.children:
                            if hasattr(child, "color"):
                                if child.color == colorIndex[tree["children"][0]["val"]]:
                                    print "found ball by color"
                                    print child.id
                                    child.color = colorIndex[tree["children"][1]["val"]]
                elif tree["val"] == "move":
                    s = tree["children"][1]["val"]
                    destination = list()
                    for item in s.split(','):
                        destination.append(int(item))
                    if tree["children"][0]["type"] == "ball":
                        for child in self.root.children:
                            if hasattr(child, "id"):
                                if child.id == int(tree["children"][0]["val"]):
                                    print "moving by id: %d" % child.id
                                    child.center = destination
                    elif tree["children"][0]["type"] == "color":
                        for child in self.root.children:
                            if hasattr(child, "color"):
                                if child.color == colorIndex[tree["children"][0]["val"]]:
                                    print "moving by color"
                                    child.center = destination

        
    def utterance_begin(self, *args):
        print "start"
        r = requests.get('http://127.0.0.1:7500/utterance/begin')
        #print r.content
        r = requests.get('http://127.0.0.1:7500/utterance/speech/begin')
        #print r.content
    
    def utterance_end(self, *args):
        global gestureBuffer
        print "gesture"
        if gestureBuffer != []:
            jsonData = json.dumps(gestureBuffer)
            print jsonData
            r = requests.post('http://127.0.0.1:7500/utterance/gesture/data', jsonData)
            gestureBuffer = []
        time.sleep(0.25)
        print "end"
        r = requests.get('http://127.0.0.1:7500/utterance/speech/end')
        #print r.content
        r = requests.get('http://127.0.0.1:7500/utterance/end')
        #print r.content
        time.sleep(0.5)
        self.utterance_get()
    

    def on_key_down(self, instance, scancode, *largs):
        global utteranceStarted
        if scancode == 32: # spacebar
            if utteranceStarted == False:
                utteranceStarted = True
                self.utterance_begin()
        elif scancode == 50:
            self.utterance_get()
        else:
            print scancode
    
    def on_key_up(self, scancode, *largs):
        global utteranceStarted
        if largs[0] == 32: # spacebar
            if utteranceStarted == True:
                utteranceStarted = False
                self.utterance_end()
    
    def on_touch_down(instance, self, touch):
        global gestureBuffer
        for child in self.children[:]:
            if child.dispatch('on_touch_down', touch):
                return True
        if utteranceStarted:
            print "Point: %d %d" % touch.pos
            gestureBuffer.append({"type" : "point", "val" : "%d,%d" % touch.pos})
        
    def build(self):
        root = self.root
        requests.get('http://127.0.0.1:7500/pipeline/start')
        Window.bind(on_key_down=self.on_key_down)
        Window.bind(on_key_up=self.on_key_up)
        root.bind(on_touch_down=self.on_touch_down)
        
    
if __name__ == '__main__':
    #Config.set('input', 'default', 'mouse')
    #Config.write()
    # do the build_config sometime
    BallWorld().run()
    requests.get('http://127.0.0.1:7500/pipeline/stop')

