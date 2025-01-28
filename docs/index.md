---
generator: doxide
---


# Untitled



## Types

| Name | Description |
| ---- | ----------- |
| [URLShortenerHandler](URLShortenerHandler.md) |  Request handler for shortening URLs. |
| [APIVersionHandler](APIVersionHandler.md) |  Handler for getting the current API version. |
| [FileRequestHandler](FileRequestHandler.md) |  Request handler for serving files from the filesystem. |
| [NotFoundHandler](NotFoundHandler.md) | Fallback handler for requests that do not match any other handler. |
| [URLRedirectHandler](URLRedirectHandler.md) |  Request handler for redirecting shortened URLs to their original form. |
| [IRequestHandler](IRequestHandler.md) |  Interface for handling HTTP requests. |
| [ObjectPool](ObjectPool.md) | This object pool is meant for use in multi-threaded environment where Object type is not a thread safe Many threads can share ObjectPool and have a unique ownership of an instance of Object with getObject call, which ensures that calling thread is the only owner of returned instance  |
| [HTTPServer](HTTPServer.md) |  HTTP server that listens on given URL and handles requests with registered handlers. |
| [KafkaReportingHttpServer](KafkaReportingHttpServer.md) |  HTTP server that listens on given URL and sends request metadata to Kafka. |
| [PostgresDBManager](PostgresDBManager.md) |  Class for managing database operations. |
| [DatabaseManager](DatabaseManager.md) |  Interface for database manager. |

