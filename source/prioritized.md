# Persistent prioritized queue (`ict::queue::prioritized`) - since v1.2

`ict::queue::prioritized` is a queue with prioritization that is stored entirely in files. New elements are added immediately to the files.

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
//! @param p Priority (from 0 - the lowest, to 255 - the highest) - input.
//! 
void push(const Container & c,const priority_t & p);
//! 
//! @brief Deletes an item from the queue.
//! 
//! @param c Item removed from the queue.
//! @param p Priority (from 0 - the lowest, to 255 - the highest) - output.
//! 
void pop(Container & c,priority_t & p);
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
* `std::string` (then `ict::queue::prioritized` or `ict::queue::prioritized_string` should be used);
* `std::wstring` (then `ict::queue::prioritized_wstring` should be used).

## Usage
```c
#inlude "libict-queue/source/prioritized.hpp"

ict::queue::prioritized queue(dirpath);//Directry must exists
std::string input("Ala ma kota!");
std::string output;
ict::queue::prioritized::priority_t p;
queue.push(input,100);//Adds element to the queue with 100 priority.
queue.pop(output,p);//Removes element from the queue (its not allowed if queue is empty).
```