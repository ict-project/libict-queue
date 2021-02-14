//! @file
//! @brief Directory pool module - Source file.
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
#include "dir-pool.hpp"
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
//============================================
namespace ict { namespace  queue { namespace  dir {
//============================================
static char value2hex(unsigned char input){
    input&=0xf;
    switch (input){
        case 0x0:return('0');
        case 0x1:return('1');
        case 0x2:return('2');
        case 0x3:return('3');
        case 0x4:return('4');
        case 0x5:return('5');
        case 0x6:return('6');
        case 0x7:return('7');
        case 0x8:return('8');
        case 0x9:return('9');
        case 0xa:return('a');
        case 0xb:return('b');
        case 0xc:return('c');
        case 0xd:return('d');
        case 0xe:return('e');
        default:return('f');
    }
}
static unsigned char hex2value(char input){
    switch (input){
        case '0':return(0x0);
        case '1':return(0x1);
        case '2':return(0x2);
        case '3':return(0x3);
        case '4':return(0x4);
        case '5':return(0x5);
        case '6':return(0x6);
        case '7':return(0x7);
        case '8':return(0x8);
        case '9':return(0x9);
        case 'a':case 'A':return(0xa);
        case 'b':case 'B':return(0xb);
        case 'c':case 'C':return(0xc);
        case 'd':case 'D':return(0xd);
        case 'e':case 'E':return(0xe);
        case 'f':case 'F':return(0xf);
        default:return(0x0);
    }
}
static std::string encodeCharForPath(char input){
    static const std::string special("()-_|."); 
    std::string output;
    if ((std::isalnum(input))||(special.find(input)!=std::string::npos)){
        output=input;
    } else {
        output='%';
        output+=value2hex(input>>4);
        output+=value2hex(input&0xf);
    }
    return(output);
}
static char decodeCharForPath(const std::string & input){
    char output=0;
    if (input.size()==3){
        output=hex2value(input[1]);
        output<<=4;
        output|=hex2value(input[2]);
    }
    return(output);
}
inline std::string encodeStringForPathLocal(const std::string & input){
    std::string output;
    for(const char & c : input){
        output+=encodeCharForPath(c);
    }
    return(output);
}
inline std::string decodeStringForPathLocal(const std::string & input){
    std::string output;
    for(std::size_t k=0;k<input.size();k++){
        if (input.at(k)=='%'){
            output+=decodeCharForPath(input.substr(k,3));
            k+=2;
        } else {
           output+=input.at(k);
        }
    }
    return(output);
}
void pool::idToString(const std::string & input,std::string & output){
    if (input.size()>100) throw std::invalid_argument("ict::queue::dir::pool id too long!");
    if (input.size()==0) throw std::invalid_argument("ict::queue::dir::pool id too short!");
    output=encodeStringForPath(input);
}
void pool::idFromString(const std::string & input,std::string & output){
    output=decodeStringForPath(input);
}
std::string pool::encodeStringForPath(const std::string & input){
    return(encodeStringForPathLocal(input));
}
std::string pool::decodeStringForPath(const std::string & input){
    return(decodeStringForPathLocal(input));
}
ict::queue::types::path_t pool::getPathString(const std::string & id) const{
    ict::queue::types::path_t output;
    output+=dir;
    output+=std::filesystem::path::preferred_separator;
    output+=id;
    output+=".q";
    return(output);
}
void pool::getAllIdsString(std::set<std::string> & output){
    if (std::filesystem::is_directory(dir)){
        ids.clear();
        std::vector<ict::queue::types::path_t> tmp;
        for(auto& p : std::filesystem::directory_iterator(dir)){
            if (std::filesystem::is_directory(p)){
                std::filesystem::path path(p);
                tmp.emplace_back(path.filename());
            }
        }
        if (tmp.size()){
            const std::regex r(R"regex(([a-zA-Z0-9%\(\)\-_\|\.]+)\.q)regex");
            for (const std::string & p : tmp){
                std::smatch m;
                if (std::regex_match(p,m,r)) {
                    if (m.size()==2){
                        std::ssub_match sm=m[1];
                        ids.emplace(sm.str());
                    }
                }
            }
        }
    } else {
        throw std::domain_error("ict::queue::dir::pool directory doesn't exist!");
    }
}
void pool::addString(const std::string & id){
    if (ids.empty()) getAllIdsString(ids);
    if (ids.count(id)) throw std::invalid_argument("ict::queue::dir::pool given id exists!");
    ids.emplace(id);
    std::filesystem::create_directory(getPathString(id));
}
void pool::removeString(const std::string & id){
    if (ids.empty()) getAllIdsString(ids);
    if (!ids.count(id)) throw std::invalid_argument("ict::queue::dir::pool given id doesn't exist!");
    ids.erase(id);
    std::filesystem::remove_all(getPathString(id));
}
bool pool::existsString(const std::string & id){
    if (ids.empty()) getAllIdsString(ids);
    return(ids.count(id));
}
pool::pool(const ict::queue::types::path_t & dirname):dir(dirname){
}
std::size_t pool::size() {
    if (ids.empty()) getAllIdsString(ids);
    return(ids.size());
}
bool pool::empty() {
    if (ids.empty()) getAllIdsString(ids);
    return(ids.empty());
}
void pool::clear(){
    if (ids.empty()) getAllIdsString(ids);
    for (const std::string & s:ids){
        std::filesystem::remove_all(getPathString(s));
    }
    ids.clear();
}
//===========================================
} } }
//===========================================
#ifdef ENABLE_TESTING
#include "test.hpp"
static ict::queue::types::path_t dirpath("/tmp/test-dirpool");
REGISTER_TEST(dirpool,tc1){
    int out=0;
    std::string test1("qwert1234~!()_|-.@#$%^&*+={}[]\\/?<>,");
    std::string test2("qwert1234%7e%21()_|-.%40%23%24%25%5e%26%2a%2b%3d%7b%7d%5b%5d%5c%2f%3f%3c%3e%2c");
    if (out==0){
        std::string tmp=ict::queue::dir::encodeStringForPathLocal(test1);
        if (tmp!=test2){
            std::cerr<<"encodeStringForPathLocal(test1)="<<tmp<<std::endl;
            std::cerr<<"test2="<<test2<<std::endl;
            out=1;
        }
    }
    if (out==0){
        std::string tmp=ict::queue::dir::decodeStringForPathLocal(test2);
        if (tmp!=test1){
            std::cerr<<"decodeStringForPathLocal(test2)="<<tmp<<std::endl;
            std::cerr<<"test1="<<test1<<std::endl;
            out=2;
        }
    }
    return(out);
}
REGISTER_TEST(dirpool,tc2){
    int out=0;
    for (size_t i=0;i<ict::test::test_string.size();i++){
        std::string test0=ict::test::test_string.at(i);
        std::string test1=ict::queue::dir::encodeStringForPathLocal(ict::test::test_string.at(i));
        std::string test2=ict::queue::dir::decodeStringForPathLocal(test1);
        if (test0!=test2){
            std::cerr<<"test0="<<test0<<std::endl;
            std::cerr<<"test1="<<test1<<std::endl;
            std::cerr<<"test2="<<test2<<std::endl;
            out=1;
            break;
        }
    }
    return(out);
}
REGISTER_TEST(dirpool,tc3){
    int out=0;
    std::set<std::string> set1={
        "sdsds",
        "ala ma kota",
        "kot ma alę",
        "qwert1234~!()_|-.@#$%^&*+={}[]\\/?<>,"
    };
    std::set<std::string> set2={
        "sdsds",
        "qwert1234~!()_|-.@#$%^&*+={}[]\\/?<>,"
    }; 
    std::filesystem::create_directory(dirpath);
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        if (pool.size()!=0){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        for (const auto & k:set1) pool.add(k);
        if (pool.size()!=set1.size()){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=2;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        std::set<std::string> s;
        pool.getAllIds(s);
        if (s!=set1){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=3;
        } else if (pool.exists("jknndd")){
            out=4;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        for (const auto & k:set2) pool.remove(k);
        if (pool.size()!=set1.size()-set2.size()){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=5;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        pool.clear();
        if (pool.size()!=0){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=6;
        }
    }
    std::filesystem::remove_all(dirpath);
    return(out);
}
REGISTER_TEST(dirpool,tc4){
    int out=0;
    std::set<int> set1={
        62625,
        228,
        99962662,
        -23
    };
    std::set<int> set2={
        228,
        -23
    }; 
    std::filesystem::create_directory(dirpath);
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        if (pool.size()!=0){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=1;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        for (const auto & k:set1) pool.add(k);
        if (pool.size()!=set1.size()){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=2;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        std::set<int> s;
        pool.getAllIds(s);
        if (s!=set1){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=3;
        } else if (pool.exists(82572)){
            out=4;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        for (const auto & k:set2) pool.remove(k);
        if (pool.size()!=set1.size()-set2.size()){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=5;
        }
    }
    if (out==0) {
        ict::queue::dir::pool pool(dirpath);
        pool.clear();
        if (pool.size()!=0){
            std::cerr<<"pool.size()="<<pool.size()<<std::endl;
            out=6;
        }
    }
    std::filesystem::remove_all(dirpath);
    return(out);
}
#endif
//===========================================