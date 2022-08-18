//! @file
//! @brief File interface module - Source file.
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
//============================================
#include "file-interface.hpp"
#include <filesystem>
//============================================
namespace ict { namespace  queue { namespace  file {
//============================================
std::size_t interface::getPositionFromFile(){
    std::size_t output=0;
    if (!fpool.empty()){
        std::ifstream s;
        ict::queue::types::record_t r;
        s.open(fpool.getPath(0),std::ios::in|std::ios::binary);
        while(s){
            s>>r;
            if (s) switch(r.type){
                case ict::queue::types::payload_size_record:
                    s.seekg(r.data,std::ios::cur);
                    break;
                case ict::queue::types::read_pointer_record:
                case ict::queue::types::read_confirm_record:
                    output=r.data;
                    break;
                case ict::queue::types::queue_size_record:
                default:break;
            };
        }
    }
    return output;
}
std::size_t interface::getSizeFromFile(){
    std::size_t output=0;
    if (!fpool.empty()){
        bool found=false;
        std::ifstream s;
        ict::queue::types::record_t r;
        s.open(fpool.getPath(0),std::ios::in|std::ios::binary);
        while(s){
            s>>r;
            if (s) switch(r.type){
                case ict::queue::types::payload_size_record:
                    s.seekg(r.data,std::ios::cur);
                    if (found) output++;
                    break;
                case ict::queue::types::read_pointer_record:
                    break;
                case ict::queue::types::read_confirm_record:
                    if (found) output--;
                    break;
                case ict::queue::types::queue_size_record:
                    found=true;
                    output=r.data;
                    break;
                default:break;
            };
        }
    }
    return output;
}
interface::interface(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize,const std::size_t & maxFiles):
    fpool(dirname,maxFileSize,maxFiles){
    refresh();
}
void interface::writeInfo(){
    {
        ict::queue::types::record_t r_size={ict::queue::types::queue_size_record,0};
        r_size.data=queue_size;
        getWriteStream()<<r_size;
    }
    {
        ict::queue::types::record_t r_pointer={ict::queue::types::read_pointer_record,0};
        if (istream) r_pointer.data=(*istream).tellg();
        getWriteStream()<<r_pointer;
    }
    getWriteStream().flush();
}
std::ofstream & interface::getWriteStream(){
    if (!ostream){
        if (fpool.empty()) throw std::underflow_error("ict::queue::file::interface is empty!");
        ostream.reset(new std::ofstream);
        ostream->open(fpool.getPath(0),std::ios::out|std::ios::binary|std::ios::app);
    }
    return *ostream;
}
std::ifstream & interface::getReadStream(){
    if (!istream){
        if (fpool.empty()) throw std::underflow_error("ict::queue::file::interface is empty!");
        istream.reset(new std::ifstream);
        istream->open(fpool.getPath(fpool.size()-1),std::ios::in|std::ios::binary);
        istream->seekg(getPositionFromFile(),std::ios::beg);
    }
    return *istream;
}
void interface::nextWriteStream(){
    ostream.reset(nullptr);
    fpool.pushFront();
    writeInfo();
}
void interface::nextReadStream(){
    istream.reset(nullptr);
    fpool.popBack();
    writeInfo();
}
std::size_t interface::size() const{
    return fpool.size();
}
bool interface::empty() const{
    return fpool.empty();
}
void interface::clear(){
    ostream.reset(nullptr);
    istream.reset(nullptr);
    fpool.clear();
    queue_size=0;
}
std::atomic_size_t & interface::queueSize(){
    return queue_size;
}
bool interface::refresh(){
    if (fpool.refresh()){
        ostream.reset(nullptr);
        istream.reset(nullptr);
        queue_size=getSizeFromFile();
        readySize=true;
        return true;
    }
    if (ostream) if (ostream->tellp()!=std::filesystem::file_size(fpool.getPath(0))){
        ostream.reset(nullptr);
        istream.reset(nullptr);
        queue_size=getSizeFromFile();
        readySize=true;
        return true;
    }
    if (!readySize){
        queue_size=getSizeFromFile();
        readySize=true;
        return true;
    }
    return false;
}
//===========================================
} } }
//===========================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include <filesystem>

static ict::queue::types::path_t dirpath("/tmp/test-fileinterface");
REGISTER_TEST(fileinterface,tc1){
    int out=0;
    std::filesystem::create_directory(dirpath);
    {
        ict::queue::file::interface iface(dirpath);
        iface.nextWriteStream();
        iface.queueSize()++;
        iface.queueSize()++;
        iface.nextWriteStream();
        iface.nextReadStream();
        iface.nextWriteStream();
        iface.nextReadStream();
        iface.getWriteStream();
        if (iface.queueSize()!=2){
            std::cerr<<"iface.queueSize()="<<iface.queueSize()<<std::endl;
            out=1;
        } else if (iface.size()!=1) {
            std::cerr<<"iface.size()="<<iface.size()<<std::endl;
            out=2;
        }
    }
    if (out==0) {
        ict::queue::file::interface iface(dirpath);
        if (iface.queueSize()!=2){
            std::cerr<<"iface.queueSize()="<<iface.queueSize()<<std::endl;
            out=3;
        } else if (iface.size()!=1) {
            std::cerr<<"iface.size()="<<iface.size()<<std::endl;
            out=4;
        }
        if (out==0){
            iface.nextWriteStream();
            iface.nextWriteStream();
            iface.queueSize()++;
            iface.nextReadStream();
            iface.queueSize()--;
            iface.queueSize()++;
            iface.nextWriteStream();
            iface.getReadStream();
            if (iface.queueSize()!=3){
                std::cerr<<"iface.queueSize()="<<iface.queueSize()<<std::endl;
                out=5;
            } else if (iface.size()!=3) {
                std::cerr<<"iface.size()="<<iface.size()<<std::endl;
                out=6;
            }
        }
    }
    std::filesystem::remove_all(dirpath);
    return out;
}
#endif
//===========================================