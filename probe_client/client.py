import plistlib

PROBE_KEY_STATUS = "status"
PROBE_KEY_PAYLOAD = "payload"
PROBE_KEY_OP = "op"

class ProbeException(Exception):
    pass

class ProbeClient:
    OP_NOP = 0
    OP_ALLOC = 1
    OP_FREE = 2
    OP_RDPTR = 3
    OP_WRPTR = 4
    OP_MEM_READ = 5
    OP_MEM_WRITE = 6
    OP_SYSCALL = 7

    def __init__(self, transport):
        self.transport = transport

    def request(self, opcode, *args):
        args = list(args)
        request = {PROBE_KEY_OP: opcode, PROBE_KEY_PAYLOAD: args}
        request_data = plistlib.dumps(request, fmt=plistlib.FMT_BINARY)
        reply_data = self.transport.request(request_data)
        reply = plistlib.loads(reply_data)
        if reply[PROBE_KEY_STATUS] != 0:
            raise ProbeException("reply status != 0")
        if PROBE_KEY_PAYLOAD in reply:
            return reply[PROBE_KEY_PAYLOAD]

    def nop(self):
        return self.request(self.OP_NOP)

    def alloc(self, size):
        return self.request(self.OP_ALLOC, size)

    def free(self, addr):
        return self.request(self.OP_FREE, addr)

    def rdptr(self, addr):
        return self.request(self.OP_RDPTR, addr)

    def wrptr(self, addr, value):
        return self.request(self.OP_WRPTR, addr, value)

    def mem_read(self, addr, size):
        return self.request(self.OP_MEM_READ, addr, size)

    def mem_write(self, addr, data):
        return self.request(self.OP_MEM_WRITE, addr, data)
    
    def syscall(self, num, *args):
        args = list(args)
        return self.request(self.OP_SYSCALL, num, args)
