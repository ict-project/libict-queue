//! @file
//! @brief Prioritized queue module - header file.
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
#ifndef _PRIORITIZED_QUEUE_HEADER
#define _PRIORITIZED_QUEUE_HEADER
//============================================
#include "pool.hpp"
#include <mutex>
#include <string>
//============================================
namespace ict { namespace  queue { 
//===========================================
template<class Queue=ict::queue::single_template<>> class prioritized_template {
private:
    //! Mutex.
    std::mutex prioritizedMutex;
public:
    typedef typename Queue::container_t container_t;
    typedef unsigned char priority_t;
    typedef ict::queue::pool_template<priority_t,Queue> pool_t;
    pool_t q;
    //! 
    //! @brief Konstruktor kolejki.
    //! 
    //! @param dirname Ścieżka do katalogu z plikami.
    //! @param maxFileSize Maksymalny rozmiar pliku, po przekroczeniu którego tworzony jest nowy plik.
    //! @param maxFiles Maksymalna liczba plików w puli.
    //! 
    prioritized_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
        q(dirname,maxFileSize,maxFiles){}
    prioritized_template():q("/tmp/invalid_argument",0,0){
        throw std::invalid_argument("ict::queue::prioritized constructor should have arguments!");
    }
    //! 
    //! @brief Dodaje element do kolejki.
    //! 
    //! @param c Element do dodania.
    //! @param p Priorytet (od 0 - najniższy, do 255 - najwyższy).
    //! 
    template <typename ... Args> void push(const container_t & c,const priority_t & p,Args ... a){
        std::lock_guard<std::mutex> lock(prioritizedMutex);
        q[p].push(c,a...);
    }
    //! 
    //! @brief Usuwa element z kolejki.
    //! 
    //! @param c Element usunięty z kolejki.
    //! @param p Priorytet (od 0 - najniższy, do 255 - najwyższy).
    //! 
    template <typename ... Args> void pop(container_t & c,priority_t & p,Args ... a){
        std::lock_guard<std::mutex> lock(prioritizedMutex);
        for (typename pool_t::reverse_iterator it=q.rbegin();it!=q.rend();++it){
            if (!it->second.empty()){
                it->second.pop(c,a...);
                p=it->first;
                if (it->second.empty()) q.erase(p);
                return;
            }
        }
        throw std::underflow_error("ict::queue::prioritized is empty???");
    }
    //! 
    //! @brief Zwraca aktualny rozmiar kolejki.
    //! 
    //! @return Rozmiar kolejki.
    //! 
    std::size_t size(){
        std::lock_guard<std::mutex> lock(prioritizedMutex);
        std::size_t s=0;
        for (typename pool_t::reverse_iterator it=q.rbegin();it!=q.rend();++it) s+=it->second.size();
        return(s);
    }
    //! 
    //! @brief Sprawdza, czy kolejka jest pusta.
    //! 
    //! @return true Jest pusta.
    //! @return false Nie jest pusta.
    //! 
    bool empty(){
        std::lock_guard<std::mutex> lock(prioritizedMutex);
        for (typename pool_t::reverse_iterator it=q.rbegin();it!=q.rend();++it) 
            if (!it->second.empty()) 
                return(false);
        return(true);
    }
    //! 
    //! @brief Czyści kolejkę.
    //! 
    void clear(){
        std::lock_guard<std::mutex> lock(prioritizedMutex);
        q.clear();
    }
};
typedef prioritized_template<> prioritized;
typedef prioritized_template<single_template<std::string>> prioritized_string;
typedef prioritized_template<single_template<std::wstring>> prioritized_wstring;
typedef pool_template<std::string,ict::queue::prioritized_string> pool_string_prioritized_string;
typedef pool_template<std::string,ict::queue::prioritized_wstring> pool_string_prioritized_wstring;
typedef pool_template<std::size_t,ict::queue::prioritized_string> pool_size_prioritized_string;
typedef pool_template<std::size_t,ict::queue::prioritized_wstring> pool_size_prioritized_wstring;
//===========================================
} }
//============================================
#endif