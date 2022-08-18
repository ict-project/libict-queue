//! @file
//! @brief Directory lock module - Source file.
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
//============================================
#include "dir-lock.hpp"
#include <unistd.h>
#include <fcntl.h>
//============================================
namespace ict { namespace  queue { namespace  dir {
//============================================
const std::string lockable::file_name="/dir.lock";
void lockable::lock(){
    if (fd<0){
        fd=::open(file_path.c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
    }
    if (fd<0) return;
    ::lockf(fd,F_LOCK,1);
}
void lockable::unlock(){
    if (fd<0) return;
    ::lockf(fd,F_ULOCK,1);
    ::close(fd);
    fd=-1;
}
void lockable::readHash(hash & h) const {
    if (fd<0) return;
    ::lseek(fd,0,SEEK_SET);
    ::read(fd,&h,sizeof(h));
}
void lockable::writeHash(const hash & h)const {
    if (fd<0) return;
    ::lseek(fd,0,SEEK_SET);
    ::write(fd,&h,sizeof(h));
}
//===========================================
} } }
//===========================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include <mutex>
#include <filesystem>
static ict::queue::types::path_t dirpath("/tmp/test-lock");
REGISTER_TEST(dir_lock,tc1){
    int out=0;
    std::filesystem::create_directory(dirpath);
    {
        ict::queue::dir::lockable::hash hin={1,2};
        ict::queue::dir::lockable::hash hout={4,5};
        ict::queue::dir::lockable flock(dirpath);
        std::lock_guard<ict::queue::dir::lockable> lg(flock);
        flock.writeHash(hin);
        flock.readHash(hout);
        if (hin.size!=hout.size) out=-1;
        if (hin.hash!=hout.hash) out=-2;
    }
    std::filesystem::remove_all(dirpath);
    return(out);
}
#endif
//===========================================