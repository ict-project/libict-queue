# Persistent queue pool (`ict::queue::pool`)

`ict::queue::pool` is a pool of [single queues](single.md) that is stored entirely in directories/files. New elements are added immediately to the directories/files.

## Interface
```c
//!
//! @brief Queue pool constructor.
//!
//! @param dirname Path to the directory with the queues (subdirectories).
//! @param maxFileSize Maximum file size, above which a new file is created in a single queue.
//! @param maxFiles The maximum number of files in the pool in a single queue.
//!
pool_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff);
//! 
//! @brief Adds an item to a queue in the pool.
//! 
//! @param c Item to add.
//! @param i The id of the queue in the pool.
//! 
void push(const container_t & c,const Identifier & i);
//! 
//! @brief Removes an item from a queue in the pool.
//! 
//! @param c Item removed from queue.
//! @param i The id of the queue in the pool.
//! 
void pop(container_t & c,const Identifier & i);
//! 
//! @brief Returns the current size of the queue in the pool.
//! 
//! @param i The id of the queue in the pool.
//! @return Current size of the queue.
//! 
std::size_t size(const Identifier & i);
//! 
//! @brief Tests if the queue in the pool is empty.
//! 
//! @param i The id of the queue in the pool.
//! @return true Is empty.
//! @return false Is not empty.
//! 
bool empty(const Identifier & i);
//! 
//! @brief Clears the queue in the pool.
//! 
//! @param i The id of the queue in the pool.
//!
void clear(const Identifier & i);
//! 
//! @brief Returns size of the pool.
//! 
//! @return Size of the pool.
//! 
std::size_t size();
//! 
//! @brief Returns true if pool is empty.
//! 
//! @return true The pool is empty.
//! @return false The pool is not empty.
//! 
bool empty();
//!
//! @brief Deletes all pool items.
//!
void clear();
```

## Identifier

Items of the pool can be accessed by a identifier:
* `std::string` - then:
    * `ict::queue::pool` or `ict::queue::pool_string_string` should be used (for `ict::queue::single_string`),
    * `ict::queue::pool_string_wstring` should be used (for `ict::queue::single_wstring`);
* `std::size_t` - then:
    * `ict::queue::pool_size_string` should be used (for `ict::queue::single_string`),
    * `ict::queue::pool_size_wstring` should be used (for `ict::queue::single_wstring`).

## Usage
```c
#inlude "libict-queue/source/pool.hpp"

ict::queue::pool pool(dirpath);//Directry must exists
std::string input("Ala ma kota!");
std::string output;
pool.push(input,"first_queue");//Adds element to the queue.
pool.pop(output,"first_queue");//Removes element from the queue (its not allowed if queue is empty).
```
