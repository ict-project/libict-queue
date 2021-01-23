# Persistent queue pool (`ict::queue::pool`)

`ict::queue::pool` is a pool of [single queues](single.md) that is stored entirely in directories/files. New elements are added immediately to the directories/files.

## Interface

The interface of the pool is based on `std::map` container.

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
//! @brief Element access operator. If the element does not exist, it will be created.
//!
//! @param k ID of the item.
//! @return Reference to the item.
//!
Value & operator[](const Key & k);
//!
//! @brief Deletes all pool items.
//!
void clear();
//!
//! @brief Deletes one element of the pool.
//!
//! @param pos Pointing to an element through an iterator.
//! @return parent_t :: iterator
//! 
typename parent_t::iterator erase(typename parent_t::const_iterator pos);
//!
//! @brief Deletes multiple consecutive pool items.
//!
//! @param first Pointing to the first element through an iterator.
//! @param last Show last element via iterator.
//! @return parent_t :: iterator
//!
typename parent_t::iterator erase(typename parent_t::const_iterator first,typename parent_t::const_iterator last);
//!
//! @brief Deletes one element of the pool.
//!
//! @param key ID of the item.
//! @return std :: size_t
//!
std::size_t erase(const Key& key);
```

## Key

Items of the pool can be accessed by a key:
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
pool["first_queue"].push(input);//Adds element to the queue.
pool["first_queue"].pop(output);//Removes element from the queue (its not allowed if queue is empty).
```
