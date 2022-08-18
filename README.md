# Persistent queue implementation [![Build Status](https://app.travis-ci.com/ict-project/libict-queue.svg?branch=main)](https://app.travis-ci.com/ict-project/libict-queue)

This code implements a queue that is stored entirely in files. New elements are added immediately to the files.
As of version v2.0, concurrent access to the directory with the queue data is possible by many independent processes.

See:
* [single](source/single.md) for more details about a single queue;
* [pool](source/pool.md) for more details about a pool of single queues;
* [prioritized](source/prioritized.md) for more details about a prioritized queue (since v1.2).

## Building instructions

```sh
make # Build library
make test # Execute all tests
make package # Create library package
make package_source  # Create source package
```