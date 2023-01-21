from abc import ABC, abstractmethod

class ProbeTransport(ABC):

    @abstractmethod
    def request(self, request):
        pass
