import socket
import struct
from .client import ProbeReplyException
from .transport import ProbeTransport

class TcpTransport(ProbeTransport):

    def __init__(self, ipaddr, port):
        self.ipaddr = ipaddr
        self.port = port
    
    def request(self, opcode, *args):
        args = list(args) + [0] * (8 - len(args))
        request_data = struct.pack(self.REQUEST_STRUCT, opcode, *args)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.ipaddr, self.port))
        s.send(request_data)
        reply_data = s.recv(struct.calcsize(self.REPLY_STRUCT))
        s.close()
        rop, status, retval = struct.unpack(self.REPLY_STRUCT, reply_data)
        if rop != opcode:
            raise ProbeReplyException("Incorrect opcode in reply!")
        if status != 0:
            raise ProbeReplyException("reply status != 0")
        return retval
