#!/Users/nem/project/tutorial/venv/bin/python

from pycallgraph import PyCallGraph
from pycallgraph import Config
from pycallgraph import GlobbingFilter
from pycallgraph.output import GraphvizOutput

import numpy as np
import cv2
from mss import mss
from PIL import Image, ImageGrab

import pyglet

BLACK = (  0,   0,   0)
WHITE = (255, 255, 255)
BLUE =  (  0,   0, 255)
GREEN = (  0, 255,   0)
RED =   (255,   0,   0)

screenshot_file_name = 'image.png'

class App:
    def __init__(self, width, height):
        self.size = width, height
        self.running = True
        self.window = pyglet.window.Window()

    def event(self):
        pass

    def main_loop(self):
        pyglet.app.run()
        while self.running:
            self.event()
            label = pyglet.text.Label('Hello, world',
                          font_name='Times New Roman',
                          font_size=36,
                          x=self.window.width//2, y=self.window.height//2,
                          anchor_x='center', anchor_y='center')
            self.window.clear()
            label.draw()

    def take_screenshot(self, screenshot_file_name):
        pass

def main_func():
    app = App(800, 600)
    app.main_loop()

if __name__ == '__main__':
    img = ImageGrab.grab(bbox=(100,10,400,780)) #bbox specifies specific region (bbox= x,y,width,height *starts top-left)
    img_np = np.array(img) #this is the array obtained from conversion
    frame = cv2.cvtColor(img_np, cv2.COLOR_BGR2GRAY)
    cv2.imshow("test", frame)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    config = Config()
    config.trace_filter = GlobbingFilter(include=[
        '*main*',
        'App*',
        'pyglet*',
    ]
    )

    output = GraphvizOutput()
    with PyCallGraph(output=output, config=config):
        main_func()
