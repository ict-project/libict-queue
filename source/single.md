# Persistent single queue (`ict::queue::single`)

`ict::queue::single` is a queue that is stored entirely in files. New elements are added immediately to the files.

## Interface
```c
//! 
//! @brief Queue object constructor.
//! 
//! @param dirname Path to the file directory.
//! @param maxFileSize The maximum file size above which a new file is created.
//! @param maxFiles The maximum number of files in the pool.
//! 
single_template(
    const ict::queue::types::path_t & dirname,
    const std::size_t & maxFileSize=1000000,
    const std::size_t & maxFiles=0xffffffff
);
//! 
//! @brief Adds an item to the queue.
//! 
//! @param c Item to add.
//! 
void push(const Container & c);
//! 
//! @brief Deletes an item from the queue.
//! 
//! @param c Item removed from the queue.
//! 
void pop(Container & c);
//! 
//! @brief Returns the current size of the queue.
//! 
//! @return Queue size.
//! 
std::size_t size();
//! 
//! @brief Checks if the queue is empty.
//! 
//! @return true It's empty.
//! @return false Not empty.
//! 
bool empty();
//! 
//! @brief Clears the queue.
//! 
void clear();
```

## Container

Items that can be stored in the queue may be:
* `std::string` (then `ict::queue::single` or `ict::queue::single_string` should be used);
* `std::wstring` (then `ict::queue::single_string` should be used).

## Usage
```c
#inlude "libict-queue/source/single.hpp"

ict::queue::single queue(dirpath);//Directry must exists
std::string input("Ala ma kota!");
std::string output;
queue.push(input);//Adds element to the queue.
queue.pop(output);//Removes element from the queue (its not allowed if queue is empty).
```