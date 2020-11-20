#!/Users/nem/project/tutorial/venv/bin/python

from pycallgraph import PyCallGraph
from pycallgraph import Config
from pycallgraph import GlobbingFilter
from pycallgraph.output import GraphvizOutput

import cProfile, pstats, io
from pstats import SortKey

import numpy as np
import cv2
from mss import mss
from PIL import Image, ImageGrab

import pyglet
from pyglet.window import mouse, key
from pyglet.window.key import Q

BLACK = (  0,   0,   0)
WHITE = (255, 255, 255)
BLUE =  (  0,   0, 255)
GREEN = (  0, 255,   0)
RED =   (255,   0,   0)

screenshot_file_name = 'image.png'

class MainWindow(pyglet.window.Window):
    def __init__(self, width, height):
        super(MainWindow, self).__init__(width=512, height=512, visible=False)
        self.running = True
        self.size = width, height

        self.set_visible()

    def on_key_press(self, symbol, modifiers):
        if symbol == key.Q:
            pyglet.app.exit()
        pass

    def on_key_release(self, symbol, modifiers):
        pass

    def on_mouse_motion(self, x, y, dx, dy):
        pass

    def on_mouse_press(self, x, y, button, modifiers):
        self.take_screenshot(screenshot_file_name)
        pass

    def on_mouse_release(self, x, y, button, modifiers):
        pass

    def on_draw(self):
        self.clear()
        label = pyglet.text.Label('Hello, world',
                        font_name='Times New Roman',
                        font_size=36,
                        x=self.width//2, y=self.height//2,
                        anchor_x='center', anchor_y='center')
        label.draw()

    def take_screenshot(self, screenshot_file_name):
        img = ImageGrab.grab(bbox=(0,0,700,780)) #bbox specifies specific region (bbox= x,y,width,height *starts top-left)
        img_np = np.array(img) #this is the array obtained from conversion
        print("test");
        # frame = cv2.cvtColor(img_np, cv2.COLOR_BGR2GRAY)

        # cv2.imshow("test", frame)
        # cv2.waitKey(1)
        # cv2.destroyAllWindows()
        pass

def fib_recursive(n):
    if n == 0 or n == 1:
        return n
    else:
        return fib_recursive(n-1) + fib_recursive(n-2)
def fib_iter(n):
    if n == 0 or n == 1:
        return n
    else:
        acc = [0] * n
        acc[0] = 0
        acc[1] = 1
        for i in range(2, n):
            acc[i] = acc[i-1] + acc[i-2]

        return acc[n-1] + acc[n-2]

def factorial_recur(n):
    if n <= 1:
        return 1
    return factorial_recur(n-1) * n

def factorial_iter(n):
    if n <= 1:
        return 1

    factor = 1
    for i in range(2, n):
        factor *= i

    return factor * n

def main_func():
    app = MainWindow(800, 600)
    pyglet.app.run()

    # print(fib_recursive(34))
    # print(fib_iter(200))

    # print(factorial_recur(1000))
    # print(factorial_iter(1000))


if __name__ == '__main__':
    pr = cProfile.Profile()
    pr.enable()

    main_func()

    pr.disable()
    s = io.StringIO()
    sortby = SortKey.CUMULATIVE
    ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats('main_window')
    print(s.getvalue())

    # config = Config()
    # # config.trace_filter = GlobbingFilter(include=[
    # #     '*main*',
    # #     'App*',
    # #     'cv2*',
    # # ]
    # # )
    # config.trace_filter = GlobbingFilter()

    # output = GraphvizOutput()
    # with PyCallGraph(output=output, config=config):
    #     main_func()

