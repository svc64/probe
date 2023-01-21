import struct

class ProbeReplyException(Exception):
    pass

class ProbeClient:
    REQUEST_STRUCT = "<9Q"
    REPLY_STRUCT = "<3Q"

    OP_NOP = 0

    def __init__(self, transport):
        self.transport = transport

    def request(self, opcode, *args):
        args = list(args) + [0] * (8 - len(args))
        request_data = struct.pack(self.REQUEST_STRUCT, opcode, *args)
        reply_data = self.transport.request(request_data)
        rop, status, retval = struct.unpack(self.REPLY_STRUCT, reply_data)
        if rop != opcode:
            raise ProbeReplyException("Incorrect opcode in reply!")
        if status != 0:
            raise ProbeReplyException("reply status != 0")
        return retval

    def nop(self):
        return self.request(self.OP_NOP)
