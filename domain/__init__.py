from abc import ABC, abstractmethod
from dataclasses import dataclass
from datetime import datetime


class FeedRepositoryError(RuntimeError):
    pass


class FeedRepositoryActiveTaskExistsError(FeedRepositoryError):
    pass


class FeedRepositoryUnknownError(FeedRepositoryError):
    pass


@dataclass
class FeedHistory:
    title: str
    started_at: datetime
    ended_at: datetime


class AbstractFeedRepository(ABC):
    @abstractmethod
    def feed(self, led_only=False):
        raise NotImplementedError()

    @abstractmethod
    def is_feed_active(self):
        raise NotImplementedError()

    @abstractmethod
    def get_feed_history(self) -> list[FeedHistory]:
        raise NotImplementedError()

# class AbstractFeedService(ABC):
#     @abstractmethod
#     def feed(self, led_only=False):
#         raise NotImplementedError()
#
#     @abstractmethod
#     def is_feed_active(self):
#         raise NotImplementedError()
