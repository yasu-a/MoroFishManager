from datetime import datetime, timezone, timedelta

from domain import AbstractFeedRepository, FeedRepositoryUnknownError, \
    FeedRepositoryActiveTaskExistsError, FeedHistory
from repository.esp_command import check_output, ESPCommandError


class ESPFeedRepository(AbstractFeedRepository):
    def feed(self, led_only=False):
        if led_only:
            command = "#led_only"
        else:
            command = "#led_and_feed"
        try:
            check_output(command)
        except ESPCommandError as e:
            if e.title == "ActiveTaskExists":
                raise FeedRepositoryActiveTaskExistsError()
            else:
                raise FeedRepositoryUnknownError()

    def is_feed_active(self):
        try:
            output = check_output("#get_feed_state")
        except ESPCommandError:
            raise FeedRepositoryUnknownError()
        else:
            return output == "true"

    def get_feed_history(self) -> list[FeedHistory]:
        try:
            output = check_output("#get_feed_history")
        except ESPCommandError:
            raise FeedRepositoryUnknownError()
        else:
            n, *values = output.split(" ")
            n = int(n)
            histories = []
            for i in range(n):
                title, timestamp = values[i].split(",")
                timestamp = datetime.fromtimestamp(int(timestamp))
                histories.append(FeedHistory(title=title, timestamp=timestamp))
            return list(reversed(histories))
