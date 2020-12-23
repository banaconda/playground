import queue

class MessageQueue:
    def __init__(self):
        self.queue_map = {}

    def add_queue(self, name):
        if name not in self.queue_map:
            self.queue_map[name] = queue.Queue()

    def send_message(self, name, msg):
        if name in self.queue_map:
            self.queue_map[name].put(msg)

    def recv_message(self, name):
        if name in self.queue_map:
            return self.queue_map[name].get()

        return None
