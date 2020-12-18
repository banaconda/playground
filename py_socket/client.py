from socket import *
from message import *

import json
import threading
import time
import queue


def recevier(s, queue1):
    while True:
        data = s.recv(1024)
        queue1.put(data)


def sender(s, queue1):
    d = {
        'data': 10,
        'data2': 20,
    }

    while True:
        s.sendall(json.dumps(d).encode('utf-8'))
        data = queue1.get()
        print(data)
        time.sleep(1)


if __name__ == '__main__':
    queue1 = queue.Queue()
    s = socket(AF_INET, SOCK_STREAM)
    s.connect(('127.0.0.1', 12345))

    t1 =threading.Thread(target=recevier, args=(s, queue1,))
    t2 =threading.Thread(target=sender, args=(s, queue1,))
    t1.start()
    t2.start()

    t1.join()
    t2.join()
