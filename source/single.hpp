//! @file
//! @brief Single queue module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @date 2012-2022
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2012-2022, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#ifndef _SINGLE_QUEUE_HEADER
#define _SINGLE_QUEUE_HEADER
//============================================
#include "types.hpp"
#include "basic.hpp"
#include "dir-lock.hpp"
#include "dir-singleton.hpp"
#include <mutex>
#include <string>
//============================================
namespace ict { namespace  queue { 
//===========================================
template<class Container=std::string> class single_template {
public:
    typedef Container container_t;
private:
    class _single_template {
    private:
        //! Kolejka.
        ict::queue::basic queue;
        //! Mutex dla zapisu.
        std::mutex writeMutex;
        //! Mutex dla odczytu.
        std::mutex readMutex;
        //! Blokowanie katalogu
        dir::lockable dirlock;
    public:
        //! 
        //! @brief Konstruktor kolejki.
        //! 
        //! @param dirname Ścieżka do katalogu z plikami.
        //! @param maxFileSize Maksymalny rozmiar pliku, po przekroczeniu którego tworzony jest nowy plik.
        //! @param maxFiles Maksymalna liczba plików w puli.
        //! 
        _single_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
            queue(dirname,maxFileSize,maxFiles),dirlock(dirname){}
        //! 
        //! @brief Dodaje element do kolejki.
        //! 
        //! @param c Element do dodania.
        //! 
        void push(const Container & c){
            std::lock_guard<std::mutex> lock(writeMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            std::size_t s=c.size()*sizeof(c[0]);
            queue.refresh();
            queue.writeSize(s);
            queue.writeContent((char*)&c[0]);
        }
        //! 
        //! @brief Usuwa element z kolejki.
        //! 
        //! @param c Element usunięty z kolejki.
        //! 
        void pop(Container & c){
            std::lock_guard<std::mutex> lock(readMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            std::size_t s;
            queue.refresh();
            queue.readSize(s);
            c.resize(s/sizeof(c[0]));
            queue.readContent((char*)&c[0]);
        }
        //! 
        //! @brief Zwraca aktualny rozmiar kolejki.
        //! 
        //! @return Rozmiar kolejki.
        //! 
        std::size_t size(){
            std::lock_guard<dir::lockable> dlock(dirlock);
            queue.refresh();
            return queue.size();
        }
        //! 
        //! @brief Sprawdza, czy kolejka jest pusta.
        //! 
        //! @return true Jest pusta.
        //! @return false Nie jest pusta.
        //! 
        bool empty(){
            std::lock_guard<dir::lockable> dlock(dirlock);
            queue.refresh();
            return queue.empty();
        }
        //! 
        //! @brief Czyści kolejkę.
        //! 
        void clear(){
            std::lock_guard<dir::lockable> dlock(dirlock);
            queue.refresh();
            queue.clear();
        }
    };
    dir::singleton<_single_template,std::size_t,std::size_t> _st;
public:
    single_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
        _st(dirname,maxFileSize,maxFiles){}
    single_template():_st("/tmp/invalid_argument",0,0){
        throw std::invalid_argument("ict::queue::single constructor should have arguments!");
    }
    //! 
    //! @brief Dodaje element do kolejki.
    //! 
    //! @param c Element do dodania.
    //! 
    void push(const Container & c){
        _st().push(c);
    }
    //! 
    //! @brief Usuwa element z kolejki.
    //! 
    //! @param c Element usunięty z kolejki.
    //! 
    void pop(Container & c){
        _st().pop(c);
    }
    //! 
    //! @brief Zwraca aktualny rozmiar kolejki.
    //! 
    //! @return Rozmiar kolejki.
    //! 
    std::size_t size(){
        return _st().size();
    }
    //! 
    //! @brief Sprawdza, czy kolejka jest pusta.
    //! 
    //! @return true Jest pusta.
    //! @return false Nie jest pusta.
    //! 
    bool empty(){
        return _st().empty();
    }
    //! 
    //! @brief Czyści kolejkę.
    //! 
    void clear(){
        _st().clear();
    }
};
typedef single_template<> single;
typedef single_template<std::string> single_string;
typedef single_template<std::wstring> single_wstring;
//===========================================
} }
//============================================
#endif