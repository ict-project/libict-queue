//! @file
//! @brief Basic queue module - Source file.
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
//============================================
#include "basic.hpp"
//============================================
namespace ict { namespace  queue {
//============================================
basic::basic(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize,const std::size_t & maxFiles):max_file_size(maxFileSize),iface(dirname,maxFiles){
}
void basic::writeSize(const std::size_t & size){
    {
        std::lock_guard<std::mutex> lock(writeMutex);
        if (writeOperation) throw std::domain_error("ict::queue::basic writeContent shuld be done now!");
        writeOperation=true;
        writeRecord.data=size;
        if ((iface.empty())||(max_file_size<iface.getWriteStream().tellp())){
            iface.nextWriteStream();
        }
        iface.getWriteStream()<<writeRecord;
    }
}
void basic::writeContent(const char * content){
    if (!content) throw std::invalid_argument("ict::queue::basic content is null!");
    {
        std::lock_guard<std::mutex> lock(writeMutex);
        if (!writeOperation) throw std::domain_error("ict::queue::basic writeSize shuld be done first!");
        if (writeRecord.data) iface.getWriteStream().write(content,writeRecord.data);
        iface.getWriteStream().flush();
        writeRecord.data=0;
    }
    iface.queueSize()++;
    {
        std::lock_guard<std::mutex> lock(writeMutex);
        writeOperation=false;
    }
}
void basic::readSize(std::size_t & size){
    if (iface.queueSize()==0) throw std::underflow_error("ict::queue::basic is empty!");
    {
        std::lock_guard<std::mutex> lock(readMutex);
        bool loop=true;
        if (readOperation) throw std::domain_error("ict::queue::basic readContent shuld be done now!");
        readOperation=true;
        while (loop){
            while(loop&&iface.getReadStream()){
                iface.getReadStream()>>readRecord;
                if (iface.getReadStream()) switch(readRecord.type){
                    case ict::queue::types::payload_size_record:
                        loop=false;
                        break;
                    case ict::queue::types::read_pointer_record:
                    case ict::queue::types::read_confirm_record:
                    case ict::queue::types::queue_size_record:
                    default:break;
                };
            }
            if (loop) {
                if (1<iface.size()) {
                    iface.nextReadStream();
                } else {
                    break;
                }
            }
        }
        if (loop) throw std::underflow_error("ict::queue::basic is empty???");
        size=readRecord.data;
    }
}
void basic::readContent(char * content){
    if (!content) throw std::invalid_argument("ict::queue::basic content is null!");
    ict::queue::types::record_t record={ict::queue::types::read_confirm_record,0};
    {
        std::lock_guard<std::mutex> lock(readMutex);
        if (!readOperation) throw std::domain_error("ict::queue::basic readSize shuld be done first!");
        if (readRecord.data) iface.getReadStream().read(content,readRecord.data);
        readRecord.data=0;
        record.data=iface.getReadStream().tellg();
    }
    {
        std::lock_guard<std::mutex> lock(writeMutex);
        if ((iface.empty())||(max_file_size<iface.getWriteStream().tellp())){
            iface.nextWriteStream();
        }
        iface.getWriteStream()<<record;
        iface.getWriteStream().flush();
    }
    iface.queueSize()--;
    {
        std::lock_guard<std::mutex> lock(readMutex);
        readOperation=false;
    }
}
std::size_t basic::size() {
    return(iface.queueSize());
}
bool basic::empty() {
    return(iface.queueSize()==0);
}
void basic::clear(){
    iface.clear();
}
//===========================================
} }
//===========================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include <experimental/filesystem>
namespace fs=std::experimental::filesystem;
static ict::queue::types::path_t dirpath("/tmp/test-basic");
REGISTER_TEST(basic,tc1){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::basic queue(dirpath,100);
        for (size_t i=0;i<ict::test::test_string.size();i++){
            queue.writeSize(ict::test::test_string.at(i).size());
            queue.writeContent(ict::test::test_string.at(i).data());
        }
        if (ict::test::test_string.size()!=queue.size()){
            std::cerr<<"test_string.size()="<<ict::test::test_string.size()<<std::endl;//27
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=1;
        }
        for (size_t i=0;i<ict::test::test_string.size();i++){
            std::size_t s;
            std::string c;
            queue.readSize(s);
            c.resize(s);
            queue.readContent(&c[0]);
            if (ict::test::test_string.at(i).size()!=s){
                std::cerr<<"test_string["<<i<<"].size()="<<ict::test::test_string.at(i).size()<<std::endl;
                std::cerr<<"s="<<s<<std::endl;
                out=1;
                break;
            }
            if (ict::test::test_string.at(i)!=c){
                std::cerr<<"test_string["<<i<<"]="<<ict::test::test_string.at(i)<<std::endl;
                std::cerr<<"c="<<c<<std::endl;
                out=2;
                break;
            }
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(basic,tc2){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::basic queue(dirpath,100);
        for (size_t i=0;i<ict::test::test_string.size();i++){
            queue.writeSize(ict::test::test_string.at(i).size());
            queue.writeContent(ict::test::test_string.at(i).data());
        }
        if (ict::test::test_string.size()!=queue.size()){
            std::cerr<<"test_string.size()="<<ict::test::test_string.size()<<std::endl;//27
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=1;
        }
    }
    if (out==0) {
        ict::queue::basic queue(dirpath,100);
        if (ict::test::test_string.size()!=queue.size()){
            std::cerr<<"test_string.size()="<<ict::test::test_string.size()<<std::endl;//27
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=2;
        }
    }
    if (out==0) {
        ict::queue::basic queue(dirpath,100);
        for (size_t i=0;i<ict::test::test_string.size();i++){
            std::size_t s;
            std::string c;
            queue.readSize(s);
            c.resize(s);
            queue.readContent(&c[0]);
            if (ict::test::test_string.at(i).size()!=s){
                std::cerr<<"test_string["<<i<<"].size()="<<ict::test::test_string.at(i).size()<<std::endl;
                std::cerr<<"s="<<s<<std::endl;
                out=3;
                break;
            }
            if (ict::test::test_string.at(i)!=c){
                std::cerr<<"test_string["<<i<<"]="<<ict::test::test_string.at(i)<<std::endl;
                std::cerr<<"c="<<c<<std::endl;
                out=4;
                break;
            }
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(basic,tc3){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::basic queue(dirpath,100);
        for (size_t i=0;i<ict::test::test_string.size();i++){
            {
                queue.writeSize(ict::test::test_string.at(i).size());
                queue.writeContent(ict::test::test_string.at(i).data());
            }
            {
                std::size_t s;
                std::string c;
                queue.readSize(s);
                c.resize(s);
                queue.readContent(&c[0]);
                if (ict::test::test_string.at(i).size()!=s){
                    std::cerr<<"test_string["<<i<<"].size()="<<ict::test::test_string.at(i).size()<<std::endl;
                    std::cerr<<"s="<<s<<std::endl;
                    out=1;
                    break;
                }
                if (ict::test::test_string.at(i)!=c){
                    std::cerr<<"test_string["<<i<<"]="<<ict::test::test_string.at(i)<<std::endl;
                    std::cerr<<"c="<<c<<std::endl;
                    out=2;
                    break;
                }
            }
        }
        if (queue.size()!=0){
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=3;
        }
    }
    if (out==0) {
        ict::queue::basic queue(dirpath,100);
        if (queue.size()!=0){
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=4;
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(basic,tc4){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::basic queue(dirpath);
        for (size_t i=0;i<ict::test::test_string.size();i++){
            {
                queue.writeSize(ict::test::test_string.at(i).size());
                queue.writeContent(ict::test::test_string.at(i).data());
            }
            {
                std::size_t s;
                std::string c;
                queue.readSize(s);
                c.resize(s);
                queue.readContent(&c[0]);
                if (ict::test::test_string.at(i).size()!=s){
                    std::cerr<<"test_string["<<i<<"].size()="<<ict::test::test_string.at(i).size()<<std::endl;
                    std::cerr<<"s="<<s<<std::endl;
                    out=1;
                    break;
                }
                if (ict::test::test_string.at(i)!=c){
                    std::cerr<<"test_string["<<i<<"]="<<ict::test::test_string.at(i)<<std::endl;
                    std::cerr<<"c="<<c<<std::endl;
                    out=2;
                    break;
                }
            }
        }
        if (queue.size()!=0){
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=3;
        }
    }
    if (out==0) {
        ict::queue::basic queue(dirpath);
        if (queue.size()!=0){
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=4;
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(basic,tc5){
    int out=0;
    fs::create_directory(dirpath);
    std::size_t half=ict::test::test_string.size()/2;
    {
        ict::queue::basic queue(dirpath,500);
        for (size_t i=0;i<ict::test::test_string.size();i++){
            queue.writeSize(ict::test::test_string.at(i).size());
            queue.writeContent(ict::test::test_string.at(i).data());
        }
        if (ict::test::test_string.size()!=queue.size()){
            std::cerr<<"test_string.size()="<<ict::test::test_string.size()<<std::endl;//27
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=1;
        }
    }
    if (out==0) {
        ict::queue::basic queue(dirpath,500);
        for (size_t i=0;i<half;i++){
            std::size_t s;
            std::string c;
            queue.readSize(s);
            c.resize(s);
            queue.readContent(&c[0]);
            if (ict::test::test_string.at(i).size()!=s){
                std::cerr<<"test_string["<<i<<"].size()="<<ict::test::test_string.at(i).size()<<std::endl;
                std::cerr<<"s="<<s<<std::endl;
                out=2;
                break;
            }
            if (ict::test::test_string.at(i)!=c){
                std::cerr<<"test_string["<<i<<"]="<<ict::test::test_string.at(i)<<std::endl;
                std::cerr<<"c="<<c<<std::endl;
                out=3;
                break;
            }
        }
    }
    if (out==0) {
        ict::queue::basic queue(dirpath,500);
        for (size_t i=half;i<ict::test::test_string.size();i++){
            std::size_t s;
            std::string c;
            queue.readSize(s);
            c.resize(s);
            queue.readContent(&c[0]);
            if (ict::test::test_string.at(i).size()!=s){
                std::cerr<<"test_string["<<i<<"].size()="<<ict::test::test_string.at(i).size()<<std::endl;
                std::cerr<<"s="<<s<<std::endl;
                out=4;
                break;
            }
            if (ict::test::test_string.at(i)!=c){
                std::cerr<<"test_string["<<i<<"]="<<ict::test::test_string.at(i)<<std::endl;
                std::cerr<<"c="<<c<<std::endl;
                out=5;
                break;
            }
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
#endif
//===========================================