from socket import *
import threading
import time
import queue

from MessageQueue import *

class Server:
    def __init__(self, host, port):
        s = socket(AF_INET, SOCK_STREAM)
        s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

        s.bind((host, port))
        s.listen()

        while True:
            conn, addr = s.accept()



def event_handler(queue1, queue2):
    while True:
        data = queue1.get()
        print('event_get_data')
        time.sleep(1)
        queue2.put(data)

        pass


def receiver(conn, queue1):
    while True:
        data = conn.recv(1024)
        if not data:
            continue

        queue1.put(data)


def sender(conn, queue2):
    while True:
        data = queue2.get()
        conn.sendall(data)


if __name__ == '__main__':
    msg_queue = MessageQueue()

    msg_queue.add_queue('')
    queue1 = queue.Queue()
    queue2 = queue.Queue()

    e = threading.Thread(target=event_handler, args=(queue1, queue2, ))
    e.start()

    s = socket(AF_INET, SOCK_STREAM)
    s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

    s.bind(('127.0.0.1', 12345))
    s.listen()

    while True:
        conn, addr = s.accept()
        print(conn, addr)
        t1 = threading.Thread(target=receiver, args=(conn, queue1, ))
        t1.start()
        t2 = threading.Thread(target=sender, args=(conn, queue2, ))
        t2.start()
