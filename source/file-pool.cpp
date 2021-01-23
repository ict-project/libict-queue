//! @file
//! @brief File pool module - Source file.
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
#include "file-pool.hpp"
#include <map>
#include <regex>
#include <fstream>
#include <sstream>
#include <iomanip>

#if defined(__cpp_lib_filesystem)
 #include <filesystem>
 namespace fs=std::filesystem;
#else
 #include <experimental/filesystem>
 namespace fs=std::experimental::filesystem;
#endif
//============================================
namespace ict { namespace  queue { namespace  file {
//============================================
pool::pool(const ict::queue::types::path_t & dirname,const std::size_t & max):dir(dirname),maxSize(max){
    loadFileList();
}
const ict::queue::types::path_t & pool::getPath(const std::size_t & index) const{
    if (index<list.size()) return(list.at(index).path);
    throw std::out_of_range("ict::queue::file::pool out of range!");
}
std::size_t pool::size() const {
    return(list.size());
}
bool pool::empty() const {
    return(list.empty());
}
void pool::pushFront(){
    if (fs::is_directory(dir)){
        item_t i;
        if (list.empty()) loadFileList();
        if (maxSize<=list.size()) throw std::overflow_error("ict::queue::file::pool max size exceeded!");
        i.number=0;
        if (!list.empty()) {
            i.number=list.at(0).number;
            i.number++;
        }
        i.path=getPathString(i.number);
        list.emplace(list.begin(),i);
        createFile(i.path);
    } else {
        throw std::domain_error("ict::queue::file::pool directory doesn't exist!");
    }
}
void pool::popBack(){
    if (fs::is_directory(dir)){
        if (list.empty()) loadFileList();
        if (!list.empty()){
            removeFile(list.back().path);
            list.pop_back();
        } else {
            throw std::underflow_error("ict::queue::file::pool is empty!");
        }
    } else {
        throw std::domain_error("ict::queue::file::pool directory doesn't exist!");
    }
}
void pool::clear(){
    if (fs::is_directory(dir)){
        if (list.empty()) loadFileList();
        while(!list.empty()){
            removeFile(list.back().path);
            list.pop_back();
        }
    } else {
        list.clear();
    }
}
ict::queue::types::path_t pool::getPathString(pool::number_t n) const{
    std::stringstream stream;
    stream<<dir<<fs::path::preferred_separator;
    stream<<std::hex<<std::noshowbase<<std::setfill('0')<<std::setw(sizeof(number_t)*2)<<n;
    stream<<".dat";
    return(stream.str());
}
std::string pool::getRegexString() const{
    std::stringstream stream;
    stream<<"([0123456789abcdef]{";
    stream<<std::dec<<std::noshowbase<<(sizeof(number_t)*2);
    stream<<"})\\.dat";
    return(stream.str());
}
void pool::loadFileList(){
    std::vector<ict::queue::types::path_t> tmp1;
    std::map<number_t,ict::queue::types::path_t> tmp2;
    number_t tmp3;
    if (fs::is_directory(dir)){
        for(auto& p: fs::directory_iterator(dir)){
            if (fs::is_regular_file(p)){
                fs::path path(p);
                tmp1.emplace_back(path.filename());
            }
        }
    }
    if (tmp1.size()){
        const std::regex r(getRegexString());
        for (std::vector<ict::queue::types::path_t>::const_iterator it=tmp1.cbegin();it!=tmp1.cend();++it){
            std::smatch m;
            if (std::regex_match(*it,m,r)) {
                if (m.size()==2){
                    std::ssub_match sm=m[1];
                    std::string s="0x";
                    s+=sm.str();
                    number_t number=std::stoull(s,nullptr,16);
                    tmp2[number]=getPathString(number);
                }
            }
        }
    }
    if (tmp2.size()){
        number_t last=tmp2.rbegin()->first;
        while (tmp2.count(last)) last++;
        last--;
        while (tmp2.count(last)) {
            list.emplace_back(item_t{tmp2[last],last});
            last--;
        }
    }
}
void pool::createFile(const ict::queue::types::path_t & path) const {
    std::ofstream f;
    if (fs::exists(path)) fs::remove(path);
    f.open(path,std::ios::out);
}
void pool::removeFile(const ict::queue::types::path_t & path) const {
    fs::remove(path);
}
//===========================================
} } }
//===========================================
#ifdef ENABLE_TESTING
#include "test.hpp"

#if defined(__cpp_lib_filesystem)
 #include <filesystem>
 namespace fs=std::filesystem;
#else
 #include <experimental/filesystem>
 namespace fs=std::experimental::filesystem;
#endif

static ict::queue::types::path_t dirpath("/tmp/test-filepool");
static void test_createFile(const ict::queue::types::path_t & path) {
    std::ofstream f;
    f.open(path,std::ios::out);
}
REGISTER_TEST(filepool,tc1){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
        pool.popBack();
        if (pool.size())  std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()!=0) {
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(filepool,tc2){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
        if (pool.size())  std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()!=1) {
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(filepool,tc3){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
        pool.popBack();
    }
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
        pool.popBack();
        if (pool.size())  std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()!=0) {
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(filepool,tc4){
    int out=0;
    fs::create_directory(dirpath);
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
    }
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
        if (pool.size())  std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()!=2) {
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(filepool,tc5){
    int out=0;
    ict::queue::types::path_t f1(dirpath);
    ict::queue::types::path_t f2(dirpath);
    fs::create_directory(dirpath);
    f1+=fs::path::preferred_separator;
    f1+="00000000000000f0.dat";
    f2+=fs::path::preferred_separator;
    f2+="00000000000000f1.dat";
    test_createFile(f1);
    test_createFile(f2);
    {
        ict::queue::file::pool pool(dirpath);
        if (pool.size()) std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()>1) std::cout<<"prev path="<<pool.getPath(1)<<std::endl;
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
    }
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
        if (pool.size())  std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()!=4) {
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }   
    }
    fs::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(filepool,tc6){
    int out=0;
    ict::queue::types::path_t f1(dirpath);
    ict::queue::types::path_t f2(dirpath);
    fs::create_directory(dirpath);
    f1+=fs::path::preferred_separator;
    f1+="fffffffffffffffd.dat";
    f2+=fs::path::preferred_separator;
    f2+="fffffffffffffffe.dat";
    test_createFile(f1);
    test_createFile(f2);
    {
        ict::queue::file::pool pool(dirpath);
        if (pool.size()) std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()>1) std::cout<<"prev path="<<pool.getPath(1)<<std::endl;
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
    }
    {
        ict::queue::file::pool pool(dirpath);
        pool.pushFront();
        pool.pushFront();
        pool.popBack();
        pool.pushFront();
        pool.popBack();
        if (pool.size())  std::cout<<"last path="<<pool.getPath(0)<<std::endl;
        if (pool.size()!=4) {
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }   
    }
    fs::remove_all(dirpath);
    return(out);
}
#endif
//===========================================