import struct

class ProbeReplyException(Exception):
    pass

class ProbeClient:

    def __init__(self, transport):
        self.transport = transport

    def request(self, opcode, *args):
        return self.transport.request(opcode, *args)
