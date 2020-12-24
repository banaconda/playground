import queue
import json
import time
import threading

from Module import Module


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
            time.sleep(1)
            self.module.tr_slot()

    def run(self):
        t = threading.Thread(target=self.tr_logic)
        t.start()

        while True:
            req = self.request_queue.get()
            self.response_queue.put(req)
            if req['name'] == 'tr':
                self.tr_queue.put(req)
