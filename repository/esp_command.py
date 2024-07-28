import os
import socket


class ESPCommandError(RuntimeError):
    def __init__(self, title: str | None, rest: str | None):
        self.title = title
        self.rest = rest


def check_output(command_str: str) -> str:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    address = str(os.environ["ESP_HOST"]), int(os.environ["ESP_PORT"])
    s.connect(address)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    s.sendall(command_str.encode("utf-8"))
    msg = s.recv(256)
    while b"\n" not in msg:
        msg += s.recv(256)
    msg = msg.decode("utf-8").strip()

    print("COMMAND", repr(command_str), "->", repr(msg))
    msg_split = msg.split(" ", maxsplit=3)
    while len(msg_split) < 3:
        msg_split += [""]

    result, *rest = msg_split
    if result == "#OK":
        return " ".join(rest).strip()
    elif result == "#NG":
        title, rest = rest
        raise ESPCommandError(title=title.strip(), rest=rest.strip())
    else:
        raise ESPCommandError(title="Unknown", rest="")


if __name__ == '__main__':
    print(check_output("#get_feed_state"))
    print(check_output("#led_only"))
    print(check_output("#get_feed_state"))
    print(check_output("#get_feed_state"))
    print(check_output("#led_and_feed"))
    print(check_output("#get_feed_state"))
