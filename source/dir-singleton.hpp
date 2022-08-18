//! @file
//! @brief Directory singleton module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @date 2022
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2022, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#ifndef _DIR_SINGLETON_HEADER
#define _DIR_SINGLETON_HEADER
//============================================
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "types.hpp"
//============================================
namespace ict { namespace  queue { namespace  dir {
//===========================================
bool equivalent(const ict::queue::types::path_t & lhs,const ict::queue::types::path_t & rhs) noexcept;
template<class T,typename ... A> class singleton {
private:
    typedef std::shared_ptr<T> ptr_t;
    typedef std::map<ict::queue::types::path_t,ptr_t> map_t;
    static std::mutex mutex;
    static map_t map;
    ptr_t ptr;
    ict::queue::types::path_t path;
public:
    singleton(const ict::queue::types::path_t & p,A ... args):path(p){
        std::lock_guard<std::mutex> lock(mutex);
        if (map.count(p)){
            ptr=map[p];
            return;
        }
        for (typename map_t::const_iterator it=map.cbegin();it!=map.cend();++it){
            if (equivalent(p,it->first)) {
                ptr=it->second;
                path=it->first;
                return;
            }
        }
        ptr.reset(new T(p,args...));
        map[p]=ptr;
    }
    ~singleton(){
        std::lock_guard<std::mutex> lock(mutex);
        ptr.reset();
        if (map.count(path)) if (map.at(path).use_count()<=1){
            map.erase(path);
        }
    }
    T & operator ()(){
        return *ptr;
    }
};
template<class T,typename ... A> std::mutex singleton<T,A ...>::mutex;
template<class T,typename ... A> typename singleton<T,A ...>::map_t singleton<T,A ...>::map;
//===========================================
} } }
//============================================
#endif