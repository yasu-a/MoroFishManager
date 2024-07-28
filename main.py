import asyncio

import uvicorn
from fastapi import FastAPI
from starlette.responses import HTMLResponse

from domain import FeedRepositoryActiveTaskExistsError, FeedRepositoryUnknownError
from provider import get_feed_repository

app = FastAPI()


def create_feed_response(msg: str):
    state = "ACTIVE" if get_feed_repository().is_feed_active() else "INACTIVE"

    histories = "".join(
        f"<li><b>{h.title}</b> - {h.timestamp!s}</li>"
        for h in get_feed_repository().get_feed_history()
    )

    page = """
    <html>
    <body>
        <h1>Feed</h1>
        <p>""" + msg + """</p>
        <p>State: """ + state + """</p>
        <p><a href="/feed/update">FEED</a></p>
        <p><a href="/feed/update?led_only=1">FEED (LED ONLY)</a></p>
        <p><a href="/feed">RELOAD</a></p>
        <h1>History</h1>
        <ol>
        """ + histories + """
        </ol>
        
    </body>
    </html>
    """
    return page


@app.get("/feed", response_class=HTMLResponse)
async def root():
    return create_feed_response(msg="")


@app.get("/feed/update", response_class=HTMLResponse)
async def feed_update(led_only: bool = False):
    try:
        get_feed_repository().feed(led_only=led_only)
    except FeedRepositoryActiveTaskExistsError:
        return create_feed_response(msg="Task is active")
    except FeedRepositoryUnknownError:
        return create_feed_response(msg="Unkown error occurred")
    else:
        return create_feed_response(msg="Feed activated successfully")


async def main():
    config = uvicorn.Config("main:app", port=5000, log_level="info")
    server = uvicorn.Server(config)
    await server.serve()


if __name__ == "__main__":
    asyncio.run(main())
