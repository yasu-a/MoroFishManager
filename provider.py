from domain import AbstractFeedRepository
from repository.esp_feed import ESPFeedRepository


def get_feed_repository() -> AbstractFeedRepository:
    return ESPFeedRepository()
