//! @file
//! @brief Queue pool module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 1.0
//! @date 2012-2021
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2012-2021, ICT-Project Mariusz Ornowski (ict-project.pl)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the ICT-Project Mariusz Ornowski nor the names
of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**************************************************************/
#ifndef _QUEUE_POOL_HEADER
#define _QUEUE_POOL_HEADER
//============================================
#include <string>
#include <map>
#include <mutex>
#include "types.hpp"
#include "dir-pool.hpp"
#include "single.hpp"
//============================================
namespace ict { namespace  queue { 
//===========================================
//! Pula kolejek
template<
    class Key=std::string,
    class Value=ict::queue::single,
    class Compare=std::less<Key>,
    class Allocator=std::allocator<std::pair<const Key,Value>>
> class pool_template:public std::map<Key,Value,Compare,Allocator>{
private:
    //! Mutex.
    std::mutex poolMutex;
    typedef std::map<Key,Value,Compare,Allocator> parent_t;
    //! Pula katalogów
    ict::queue::dir::pool dirs;
    //! Maksymalny 
    const std::size_t max_file_size;
    const std::size_t max_files;
public:
    //! 
    //! @brief Konstruktor puli kolejek.
    //! 
    //! @param dirname Ścieżka do katalogu z kolejkami (podkatalogi).
    //! @param maxFileSize Maksymalny rozmiar pliku, po przekroczeniu którego tworzony jest nowy plik w pojedynczej kolejce.
    //! @param maxFiles Maksymalna liczba plików w puli w pojedynczej kolejce.
    //! 
    pool_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
        dirs(dirname),max_file_size(maxFileSize),max_files(maxFiles){
        std::lock_guard<std::mutex> lock(poolMutex);
        std::set<Key> keys;
        dirs.getAllIds(keys);
        for (const Key & k:keys) {
            parent_t::emplace(
                std::piecewise_construct,
                std::forward_as_tuple(k),
                std::forward_as_tuple(dirs.getPath(k),max_file_size,max_files)
            );
        }
    }
    pool_template():dirs("/tmp/invalid_argument"),max_file_size(0),max_files(0){
        throw std::invalid_argument("ict::queue::pool constructor should have arguments!");
    }
    //! 
    //! @brief Operator dostępu do elementów. Jeśli element nie istnieje, to zostanie utworzony.
    //! 
    //! @param k ID elementu.
    //! @return Referencja do elementu.
    //! 
    Value & operator[](const Key & k){
        std::lock_guard<std::mutex> lock(poolMutex);
        if (!parent_t::count(k)){
            dirs.add(k);
            parent_t::emplace(
                std::piecewise_construct,
                std::forward_as_tuple(k),
                std::forward_as_tuple(dirs.getPath(k),max_file_size,max_files)
            );
        }
        return(parent_t::operator[](k));
    }
    //! 
    //! @brief Kasuje wszystkie elemenyty puli.
    //! 
    void clear(){
        std::lock_guard<std::mutex> lock(poolMutex);
        parent_t::clear();
        dirs.clear();
    }
    //! 
    //! @brief Kasuje jeden element puli.
    //! 
    //! @param pos Wskazanie elementu poprzez iterator.
    //! @return parent_t::iterator 
    //! 
    typename parent_t::iterator erase(typename parent_t::const_iterator pos){
        std::lock_guard<std::mutex> lock(poolMutex);
        typename parent_t::iterator out=parent_t::erase(pos);
        dirs.remove(pos->first);
        return(out);
    }
    //! 
    //! @brief Kasuje wiele kolejnych elementów puli.
    //! 
    //! @param first Wskazanie pierwszego elementu poprzez iterator.
    //! @param last Wskazanie ostatniego elementu poprzez iterator.
    //! @return parent_t::iterator 
    //! 
    typename parent_t::iterator erase(typename parent_t::const_iterator first,typename parent_t::const_iterator last){
        std::lock_guard<std::mutex> lock(poolMutex);
        typename parent_t::iterator out=parent_t::erase(first,last);
        for (typename parent_t::const_iterator it=first;it<last;++it) dirs.remove(it->first);
        return(out);
    }
    //! 
    //! @brief Kasuje jeden element puli.
    //! 
    //! @param key ID elementu.
    //! @return std::size_t 
    //! 
    std::size_t erase(const Key& key){
        std::lock_guard<std::mutex> lock(poolMutex);
        std::size_t out=parent_t::erase(key);
        dirs.remove(key);
        return(out);
    }
};
typedef pool_template<> pool;
typedef pool_template<std::string,ict::queue::single_string> pool_string_string;
typedef pool_template<std::string,ict::queue::single_wstring> pool_string_wstring;
typedef pool_template<std::size_t,ict::queue::single_string> pool_size_string;
typedef pool_template<std::size_t,ict::queue::single_wstring> pool_size_wstring;
typedef pool_template<std::string,pool> pool2;
//===========================================
} }
//============================================
#endif