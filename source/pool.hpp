//! @file
//! @brief Queue pool module - header file.
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
#ifndef _QUEUE_POOL_HEADER
#define _QUEUE_POOL_HEADER
//============================================
#include <string>
#include <map>
#include <mutex>
#include <functional>
#include <memory>
#include "types.hpp"
#include "dir-pool.hpp"
#include "dir-lock.hpp"
#include "single.hpp"
//============================================
namespace ict { namespace  queue { 
//===========================================
//! Pula kolejek
template <typename Identifier=std::size_t,class Queue=ict::queue::single> 
class pool_template {
public:
    typedef Identifier identifier_t;
    typedef typename Queue::container_t container_t;
    typedef Queue queue_t;
    typedef std::unique_ptr<Queue> queue_ptr_t;
    typedef std::map<Identifier,queue_ptr_t> queues_t;
    class queue_info_t{
    private:
        dir::lockable & dirlock;
        dir::lockable::hash hash;
        //! Zmiana w puli katalogów.
        bool dirs_change=false;
    public:
        //! Maksymalny rozmiar pliku, po przekroczeniu którego utwprzony zostaje nowy plik.
        const std::size_t max_file_size;
        //! Maksymalna liczba plików.
        const std::size_t max_files;
        //! Pula katalogów
        ict::queue::dir::pool dirs;
        //! Lista obiektów obsługujących kolejki.
        queues_t queues;
        //! Identyfikator, który ma być użyty.
        identifier_t id;
        //! Rozmiar kolejki, który ma zostać zwrócony.
        std::size_t size;
        //! Konstruktor.
        queue_info_t(dir::lockable & dl,const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
            dirlock(dl),dirs(dirname),max_file_size(maxFileSize),max_files(maxFiles){
        }
        //! 
        //! @brief Dodaje nową kolejkę do puli (jeśli jeszcze nie istnieje).
        //! 
        //! @param i Identyfikator kolejki w puli.
        //! 
        void addQueue(const Identifier & i){
            if (!dirs.exists(i)) {
                dirs.add(i);
                dirs_change=true;
            }
            if (!queues.count(i)) {
                queues[i].reset(new Queue(dirs.getPath(i),max_file_size,max_files));
            }
        }
        //! 
        //! @brief Usuwa kolejkę z puli (jeśli istnieje).
        //! 
        //! @param i Identyfikator kolejki w puli.
        //! 
        void removeQueue(const Identifier & i){
            if (queues.count(i)) {
                queues.erase(i);
                dirs_change=true;
            }
            if (dirs.exists(i)) {
                dirs.remove(i);
            }
        }
        std::set<identifier_t> ids;
        void getAllIds(){
            if (hashChange()) dirs.getAllIds(ids);
        }
        void hashCalc(){
            std::hash<identifier_t> h; 
            hash.size=ids.size();
            hash.hash=1;
            for (const identifier_t & i : ids) {
                hash.hash=h(i);
            }
        }
        bool hashChange(){
            dir::lockable::hash read_hash;
            dirlock.readHash(read_hash);
            return (hash.hash!=read_hash.hash)||(hash.size!=read_hash.size);
        }
        void beforeChange(){
            dirs_change=false;
        }
        void afterChange(){
            if (dirs_change){
                dirs.getAllIds(ids);
                hashCalc();
                dirlock.writeHash(hash);
            }
        }
    };
    typedef std::function<void(queue_info_t &)> exec_fun_t;
protected:
    class _pool_template {
    private:
        //! Mutex.
        std::mutex poolMutex;
        //! Blokowanie katalogu
        dir::lockable dirlock;
        queue_info_t qi;
    public:
        _pool_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
            dirlock(dirname),qi(dirlock,dirname,maxFileSize,maxFiles){
        }
        //! 
        //! @brief Dodaje element do kolejki w puli.
        //! 
        //! @param c Element do dodania.
        //! @param fun Funkcja wykonawcza (wykonywana przed zwróceniem wartości).
        //! 
        template<typename ... Args> void push(const container_t & c,const exec_fun_t & fun, Args ... args){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            qi.beforeChange();
            fun(qi);
            qi.addQueue(qi.id);
            qi.queues[qi.id]->push(c,args ...);
            qi.afterChange();
        }
        //! 
        //! @brief Usuwa element z kolejki w puli.
        //! 
        //! @param c Element usunięty z kolejki.
        //! @param fun Funkcja wykonawcza (wykonywana przed zwróceniem wartości).
        //! 
        template<typename ... Args> void pop(container_t & c,const exec_fun_t & fun, Args ... args){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            qi.beforeChange();
            fun(qi);
            if (!qi.dirs.exists(qi.id)) throw std::underflow_error("Queue is empty in ict::queue::pool!");
            qi.addQueue(qi.id);
            qi.queues[qi.id]->pop(c,args ...);
            if (qi.queues[qi.id]->empty(args ...)){
                qi.removeQueue(qi.id);
            }
            qi.afterChange();
        }
        //! 
        //! @brief Zwraca aktualny rozmiar kolejki w puli.
        //! 
        //! @param fun Funkcja wykonawcza (wykonywana przed zwróceniem wartości).
        //! @return Rozmiar kolejki.
        //! 
        template<typename ... Args> std::size_t size(const exec_fun_t & fun, Args ... args){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            fun(qi);
            return qi.size;
        }
        //! 
        //! @brief Sprawdza, czy kolejka w puli jest pusta.
        //! 
        //! @param fun Funkcja wykonawcza (wykonywana przed zwróceniem wartości).
        //! @return true Jest pusta.
        //! @return false Nie jest pusta.
        //! 
        template<typename ... Args> bool empty(const exec_fun_t & fun, Args ... args){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            fun(qi);
            return qi.size==0;
        }
        //! 
        //! @brief Czyści kolejkę w puli.
        //! 
        //! @param i Identyfikator kolejki w puli.
        //!
        template<typename ... Args> void clear(const exec_fun_t & fun, Args ... args){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            qi.beforeChange();
            fun(qi);
            qi.removeQueue(qi.id);
            qi.afterChange();
        }
        //! 
        //! @brief Zwraca aktualny rozmiar całej puli kolejek.
        //! 
        //! @return Rozmiar puli kolejek.
        //! 
        std::size_t size(){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            std::size_t out=0;
            qi.getAllIds();
            for (const identifier_t & i : qi.ids) {
                qi.addQueue(i);
                out+=qi.queues[i]->size();
            }
            return out;
        }
        //! 
        //! @brief Sprawdza, czy pula kolejek jest pusta.
        //! 
        //! @return true Jest pusta.
        //! @return false Nie jest pusta.
        //! 
        bool empty(){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            qi.getAllIds();
            for (const identifier_t & i : qi.ids) {
                qi.addQueue(i);
                if (!qi.queues[i]->empty()) return false;
            }
            return true;
        }
        //! 
        //! @brief Czyści całą pulę.
        //! 
        void clear(){
            std::lock_guard<std::mutex> lock(poolMutex);
            std::lock_guard<dir::lockable> dlock(dirlock);
            qi.beforeChange();
            qi.queues.clear();
            qi.dirs.clear();
            qi.afterChange();
        }
    };
    dir::singleton<_pool_template,std::size_t,std::size_t> _pt;
public:
    pool_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
        _pt(dirname,maxFileSize,maxFiles){
    }
    //! 
    //! @brief Dodaje element do kolejki w puli.
    //! 
    //! @param c Element do dodania.
    //! @param i Identyfikator kolejki w puli.
    //! 
    template<typename ... Args> void push(const container_t & c,const Identifier & i, Args ... args){
        _pt().push(c,[&](queue_info_t & _qi){_qi.id=i;},args ...);
    }
    //! 
    //! @brief Usuwa element z kolejki w puli.
    //! 
    //! @param c Element usunięty z kolejki.
    //! @param i Identyfikator kolejki w puli.
    //! 
    template<typename ... Args> void pop(container_t & c,const Identifier & i, Args ... args){
        _pt().pop(c,[&](queue_info_t & _qi){_qi.id=i;},args ...);
    }
    //! 
    //! @brief Zwraca aktualny rozmiar kolejki w puli.
    //! 
    //! @param i Identyfikator kolejki w puli.
    //! @return Rozmiar kolejki.
    //! 
    template<typename ... Args> std::size_t size(const Identifier & i, Args ... args){
        return _pt().size([&](queue_info_t & _qi){
            _qi.id=i;
            if (_qi.dirs.exists(_qi.id)) {
                _qi.addQueue(_qi.id);
                _qi.size=_qi.queues[_qi.id]->size(args ...);
            } else {
                _qi.size=0;
            }
        },args ...);
    }
    //! 
    //! @brief Sprawdza, czy kolejka w puli jest pusta.
    //! 
    //! @param i Identyfikator kolejki w puli.
    //! @return true Jest pusta.
    //! @return false Nie jest pusta.
    //! 
    template<typename ... Args> bool empty(const Identifier & i, Args ... args){
        return _pt().empty([&](queue_info_t & _qi){
            _qi.id=i;
            if (_qi.dirs.exists(_qi.id)) {
                _qi.addQueue(_qi.id);
                _qi.size=_qi.queues[_qi.id]->size(args ...);
            } else {
                _qi.size=0;
            }
        },args ...);
    }
    //! 
    //! @brief Czyści kolejkę w puli.
    //! 
    //! @param i Identyfikator kolejki w puli.
    //!
    template<typename ... Args> void clear(const Identifier & i, Args ... args){
        _pt().clear([&](queue_info_t & _qi){_qi.id=i;},args ...);
    }
    //! 
    //! @brief Zwraca aktualny rozmiar całej puli kolejek.
    //! 
    //! @return Rozmiar puli kolejek.
    //! 
    std::size_t size(){
        return _pt().size();
    }
    //! 
    //! @brief Sprawdza, czy pula kolejek jest pusta.
    //! 
    //! @return true Jest pusta.
    //! @return false Nie jest pusta.
    //! 
    bool empty(){
        return _pt().empty();
    }
    //! 
    //! @brief Czyści całą pulę.
    //! 
    void clear(){
        _pt().clear();
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