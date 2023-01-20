from abc import ABC, abstractmethod

class ProbeTransport(ABC):

    REQUEST_STRUCT = "<9Q"
    REPLY_STRUCT = "<3Q"

    @abstractmethod
    def request(self, opcode, *args):
        pass
