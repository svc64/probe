import socket
from .client import ProbeClient
from .transport import ProbeTransport

class TcpTransport(ProbeTransport):
    def __init__(self, ipaddr, port):
        self.ipaddr = ipaddr
        self.port = port
    
    def request(self, request):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.ipaddr, self.port))
        s.send(len(request).to_bytes(4, byteorder="little"))
        s.send(request)
        respLen = s.recv(4)
        respLen = int.from_bytes(respLen, "little")
        reply_data = s.recv(respLen)
        s.close()
        return reply_data
