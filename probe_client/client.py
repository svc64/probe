import struct

class ProbeReplyException(Exception):
    pass

class ProbeClient:

    OP_NOP = 0

    def __init__(self, transport):
        self.transport = transport

    def request(self, opcode, *args):
        return self.transport.request(opcode, *args)

    def nop(self):
        return self.request(self.OP_NOP)
