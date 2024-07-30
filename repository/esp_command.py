import os
import socket
import time
from typing import Sequence


class ESPCommandError(RuntimeError):
    def __init__(self, *, title: str | None, content: str | None):
        self.title = title
        self.content = content

    def __str__(self):
        return f"{self.__class__.__name__}(title={self.title!r}, content={self.content!r})"


class ESPConnectionError(RuntimeError):
    pass


class ESPTimeoutError(RuntimeError):
    pass


class ESPCheckOutput:
    TIMEOUT_SECONDS = 4

    def __init__(self, *, name: str, args: Sequence[str]):
        self._s = None
        self._recv_buf = b""
        self._name = name
        self._args = args

    def _initialize_socket(self):
        self._s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._s.settimeout(self.TIMEOUT_SECONDS)
        address = str(os.environ["ESP_HOST"]), int(os.environ["ESP_PORT"])
        try:
            self._s.connect(address)
        except TimeoutError:
            raise ESPConnectionError()

    def _sock_send_line(self, line: str):
        assert isinstance(line, str), line
        self._s.sendall(line.encode("utf-8") + b"\n")
        # print("Send", line)

    def _sock_recv_line(self, return_byte_length=False) -> str | tuple[int, str]:
        t_start = time.time()
        while True:
            try:
                data_bytes = self._s.recv(256)
            except socket.timeout:
                pass
            else:
                data_bytes = data_bytes.replace(b"\r\n", b"\n")
                self._recv_buf += data_bytes
            i = self._recv_buf.find(b"\n")
            if i >= 0:
                break
            if time.time() - t_start >= self.TIMEOUT_SECONDS:
                raise ESPTimeoutError()

        # pop line
        line = self._recv_buf[:i].strip()
        self._recv_buf = self._recv_buf[i + 1:]

        # print("Recv", line)
        if return_byte_length:
            return len(line), line.decode("utf-8")
        else:
            return line.decode("utf-8")

    def _send_command(self):
        self._sock_send_line("#COM")
        self._sock_send_line(self._name)
        self._sock_send_line(str(len(self._args)))
        for arg in self._args:
            self._sock_send_line(arg)

    def _recv_ack_or_arq(self) -> bool:
        while True:
            line = self._sock_recv_line()
            if line == "#ACK":
                return True
            if line == "#ARQ":
                return False

    def _recv_result(self) -> tuple[str, str]:  # result, content
        while True:
            result = self._sock_recv_line()
            content_length = int(self._sock_recv_line())
            actual_content_length, content = self._sock_recv_line(return_byte_length=True)
            if content_length != actual_content_length:
                self._sock_send_line("#ARQ")
                continue

            return result, content

    def check_output(self) -> str:
        self._initialize_socket()

        try:
            while True:
                self._send_command()
                if not self._recv_ack_or_arq():
                    continue
                break

            self._sock_send_line("#RUN")

            result, content = self._recv_result()
            if result == "#OK":
                return content
            elif result.startswith("#NG:"):
                _, title = result.split(":", maxsplit=1)
                raise ESPCommandError(title=title, content=content)
            else:
                raise ESPCommandError(title="Unknown", content=content)
        finally:
            self._s.close()


def check_output(name: str, *args: str) -> str:
    return ESPCheckOutput(name=name, args=args).check_output()
