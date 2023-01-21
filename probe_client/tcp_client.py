import socket
import struct
from .client import ProbeClient
from .transport import ProbeTransport

class TcpTransport(ProbeTransport):

    def __init__(self, ipaddr, port):
        self.ipaddr = ipaddr
        self.port = port
    
    def request(self, request):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.ipaddr, self.port))
        s.send(request)
        reply_data = s.recv(struct.calcsize(ProbeClient.REPLY_STRUCT))
        s.close()
        return reply_data
