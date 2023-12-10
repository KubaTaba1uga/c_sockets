import signal
import socket
import sys
from threading import Thread
from time import sleep

FIBBONACI_SEQ_N = 10
ADDR = ("", 8080)  # all interfaces, port 8080

thread_data = {"is_alive": True}


def main():
    Thread(target=monitor).start()

    count_req_per_sec(ADDR)


def count_req_per_sec(addr):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    s.connect(addr)

    while True:
        s.send(str(FIBBONACI_SEQ_N).encode("ascii"))

        s.recv(100)

        Counter.increment()

    s.close()


def monitor():
    while thread_data["is_alive"]:
        print("Requests per second:", Counter.i)
        Counter.reset()

        sleep(1)


class Counter:
    i = 0

    @classmethod
    def increment(cls):
        cls.i += 1

    @classmethod
    def reset(cls):
        cls.i = 0


if __name__ == "__main__":

    def signal_handler(*_, **__):
        thread_data["is_alive"] = False
        sys.exit(0)

    signal.signal(signal.SIGINT, signal_handler)

    main()
