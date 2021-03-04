# Persistent queue implementation [![Build Status](https://travis-ci.org/ict-project/libict-queue.svg?branch=main)](https://travis-ci.org/ict-project/libict-queue)

This code implements a queue that is stored entirely in files. New elements are added immediately to the files.

See:
* [single](source/single.md) for more details about a single queue;
* [pool](source/pool.md) for more details about a pool of single queues;
* [prioritized](source/prioritized.md) for more details about a prioritized queue (since v1.2).

Releases:
* Branch release-v1 [![Build Status](https://travis-ci.org/ict-project/libict-queue.svg?branch=release-v1)](https://travis-ci.org/ict-project/libict-queue)

## Building instructions

```sh
make # Build library
make test # Execute all tests
make package # Create library package
make package_source  # Create source package
```