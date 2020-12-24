import queue
import threading
import time

from Server import *
from External import *


class Module:
    def __init__(self, host, port, request_queue, response_queue):
        self.host = host
        self.port = port
        self.msg_queue = queue.Queue()

        self.request_queue = request_queue
        self.response_queue = response_queue

    def run(self):
        server_thread = threading.Thread(
            target=self.server, args=(self.host, self.port, ))
        server_thread.start()

        msg_handler_thread = threading.Thread(target=self.msg_handler)
        msg_handler_thread.start()

        while True:
            pass

    def server(self, host, port):
        s = socket(AF_INET, SOCK_STREAM)
        s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

        s.bind((host, port))
        s.listen()

        while True:
            conn, addr = s.accept()
            client_queue = queue.Queue()

            client_receive_thread = threading.Thread(
                target=self.client_receiver, args=(conn, client_queue))
            client_receive_thread.start()

            client_send_thread = threading.Thread(
                target=self.client_sender, args=(conn, client_queue))
            client_send_thread.start()

    def client_receiver(self, conn: socket, client_queue):
        while True:
            data = conn.recv(65535)
            if not data:
                client_queue.put({'cmd': 'exit'})
                break
            msg = json.loads(data)
            msg['queue'] = client_queue
            self.msg_queue.put(msg)

    def client_sender(self, conn: socket, client_queue):
        while True:
            msg = client_queue.get()
            if 'cmd' in msg and msg['cmd'] == 'exit':
                print('exit')
                break

            del msg['queue']
            conn.sendall(json.dumps(msg).encode('utf-8'))

    ############

    def msg_handler(self):
        while True:
            msg = self.msg_queue.get()
            client_queue = msg['queue']
            cmd = msg['cmd']
            data = msg['data']

            # handle message
            if cmd == 'get_min_chart':
                self.get_min_chart(data['code'])

            client_queue.put(msg)

    def get_min_chart(self, code: str):
        pass


    #############
    def dyn_call(self, func, *args):
        self.request_queue.put()
        pass

    def tr_slot(self):
        print('tr')
        pass


if __name__ == '__main__':
    request_queue = queue.Queue()
    response_queue = queue.Queue()

    m = Module('127.0.0.1', 12345, request_queue, response_queue)

    e = External(m, request_queue, response_queue)
    t = threading.Thread(target=e.run)
    t.start()

    m.run()

    # while True:
    #     data = {
    #         'name': 'tr'
    #     }
    #     request_queue.put(data)
    #     time.sleep(0.5)
    #     print(response_queue.get())
