from .client import ProbeClient

class ProbeCaller:
    def __init__(self, client: ProbeClient):
        self.client = client

    def __getattribute__(self, name: str):
        client = super().__getattribute__("client")
        sym = client.dlsym(name)
        def f(*args):
            return client.fcall(sym, *args)
        return f
