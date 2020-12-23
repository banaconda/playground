import queue
import json
import time
import threading

from Module import *


class External:
    def __init__(self, module: Module, request_queue: queue.Queue, response_queue: queue.Queue):
        self.module = module
        self.request_queue = request_queue
        self.response_queue = response_queue

        self.tr_queue = queue.Queue()
        pass

    def tr_logic(self):
        while True:
            data = self.tr_queue.get()
            req = json.dumps(data)
            time.sleep(1)
            self.module.tr_slot()

    def run(self):
        t = threading.Thread(target=self.tr_logic)
        t.start()

        while True:
            data = self.request_queue.get()
            req = json.dumps(data)
            self.response_queue.put(req)
            if req['name'] == 'tr':
                self.tr_queue.put(req)


if __name__ == '__main__':
    request_queue = queue.Queue()
    response_queue = queue.Queue()
    m = Module()

    e = External(m, request_queue, response_queue)
    e.run()
