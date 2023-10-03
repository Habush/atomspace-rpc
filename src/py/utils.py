# Author Abdulrahman S. Omar <xabush@singularitynet.io>
import time
import datetime

class Timer(object):
    def __init__(self, name=None, filename=None):
        self.name = name
        self.filename = filename

    def __enter__(self):
        self.tstart = time.time()

    def __exit__(self, type, value, traceback):
        message = 'Elapsed: %.2f seconds' % (time.time() - self.tstart)
        if self.name:
            message = '[%s] ' % self.name + message
        print(message)
        if self.filename:
            with open(self.filename, 'a') as file:
                print(str(datetime.datetime.now()) + ": ", message, file=file)
