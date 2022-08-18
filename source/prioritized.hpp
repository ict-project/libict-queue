//! @file
//! @brief Prioritized queue module - header file.
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
#ifndef _PRIORITIZED_QUEUE_HEADER
#define _PRIORITIZED_QUEUE_HEADER
//============================================
#include "pool.hpp"
#include <mutex>
#include <string>
//============================================
namespace ict { namespace  queue { 
//===========================================
template<class Queue=ict::queue::single_template<>> class prioritized_template : public pool_template<unsigned char,Queue> {
public:
    typedef pool_template<unsigned char,Queue> parent_t;
    typedef typename Queue::container_t container_t;
    typedef unsigned char priority_t;
    prioritized_template(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff):
        parent_t(dirname,maxFileSize,maxFiles){
    }
    prioritized_template():parent_t("/tmp/invalid_argument",0,0){
        throw std::invalid_argument("ict::queue::prioritized constructor should have arguments!");
    }
    //! 
    //! @brief Zwraca element o najwyższym priorytecie.
    //! 
    //! @param c Element usunięty z kolejki.
    //! @param p Priorytet (od 0 - najniższy, do 255 - najwyższy). 
    //! 
    template<typename ... Args> void pop(container_t & c,priority_t & p, Args ... args){
        parent_t::_pt().pop(c,[&](typename parent_t::queue_info_t & _qi){
                _qi.getAllIds();
                if (_qi.ids.empty()) throw std::underflow_error("Queue is empty!");
                _qi.id=*_qi.ids.crbegin();
                p=_qi.id;
            },args ...
        );
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