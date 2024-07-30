import asyncio

import uvicorn
from fastapi import FastAPI, status
from fastapi.responses import RedirectResponse
from starlette.responses import HTMLResponse

from domain import FeedRepositoryActiveTaskExistsError, FeedRepositoryUnknownError
from provider import get_feed_repository

app = FastAPI()


def create_feed_response(msg: str):
    state = "ACTIVE" if get_feed_repository().is_feed_active() else "INACTIVE"

    histories = "".join(
        f"<li><b>{h.started_at!s}</b> - {h.title}</li>"
        for h in get_feed_repository().get_feed_history()
    )

    page = """
    <html>
    <link rel="stylesheet" href="https://cdn.simplecss.org/simple.min.css">
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


@app.get("/")
async def root():
    return RedirectResponse("/feed", status_code=status.HTTP_303_SEE_OTHER)


@app.get("/feed", response_class=HTMLResponse)
async def root(msg: str = ""):
    return create_feed_response(msg=msg)


@app.get("/feed/update", response_class=HTMLResponse)
async def feed_update(led_only: bool = False):
    try:
        get_feed_repository().feed(led_only=led_only)
    except FeedRepositoryActiveTaskExistsError:
        return RedirectResponse(
            "/feed?msg=" + "Task is active",
            status_code=status.HTTP_303_SEE_OTHER,
        )
    except FeedRepositoryUnknownError:
        return RedirectResponse(
            "/feed?msg=" + "Unknown error occurred",
            status_code=status.HTTP_303_SEE_OTHER,
        )
    else:
        return RedirectResponse(
            "/feed?msg=" + "Feed activated successfully",
            status_code=status.HTTP_303_SEE_OTHER,
        )


async def main():
    config = uvicorn.Config("main:app", port=5000, log_level="info")
    server = uvicorn.Server(config)
    await server.serve()


if __name__ == "__main__":
    asyncio.run(main())
