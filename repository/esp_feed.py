from datetime import datetime

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
            values = output.split(" ")
            histories = []
            for value in values:
                value = value.strip()
                if not value:
                    continue
                title, started_at, ended_at = value.split(",")
                started_at = datetime.fromtimestamp(int(started_at))
                ended_at = datetime.fromtimestamp(int(ended_at))
                histories.append(FeedHistory(title=title, started_at=started_at, ended_at=ended_at))
            return list(reversed(histories))
