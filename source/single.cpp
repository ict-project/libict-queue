//! @file
//! @brief Single queue module - Source file.
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
#include "single.hpp"
//============================================
namespace ict { namespace  queue {
//============================================

//===========================================
} }
//===========================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include <chrono>
#include <filesystem>

static ict::queue::types::path_t dirpath("/tmp/test-single");
REGISTER_TEST(single,tc1){
    int out=0;
    std::filesystem::remove_all(dirpath);
    std::filesystem::create_directory(dirpath);
    {
        ict::queue::single queue(dirpath,100);
        for (size_t i=0;i<ict::test::test_string.size();i++){
            queue.push(ict::test::test_string.at(i));
        }
        if (ict::test::test_string.size()!=queue.size()){
            std::cerr<<"test_string.size()="<<ict::test::test_string.size()<<std::endl;//27
            std::cerr<<"queue.size()="<<queue.size()<<std::endl;
            out=1;
        }
        for (size_t i=0;i<ict::test::test_string.size();i++){
            std::string c;
            queue.pop(c);
            if (ict::test::test_string.at(i)!=c){
                std::cerr<<"test_string["<<i<<"]="<<ict::test::test_string.at(i)<<std::endl;
                std::cerr<<"c="<<c<<std::endl;
                out=2;
                break;
            }
        }
    }
    std::filesystem::remove_all(dirpath);
    return out;
}
REGISTER_TEST(single,tc2){
    int out=0;
    std::filesystem::remove_all(dirpath);
    std::filesystem::create_directory(dirpath);
    {
        ict::queue::single_wstring queue(dirpath,100);
        for (size_t i=0;i<ict::test::test_wstring.size();i++){
            queue.push(ict::test::test_wstring.at(i));
        }
        if (ict::test::test_wstring.size()!=queue.size()){
            std::wcerr<<L"test_string.size()="<<ict::test::test_wstring.size()<<std::endl;//27
            std::wcerr<<L"queue.size()="<<queue.size()<<std::endl;
            out=1;
        }
        for (size_t i=0;i<ict::test::test_wstring.size();i++){
            std::wstring c;
            queue.pop(c);
            if (ict::test::test_wstring.at(i)!=c){
                std::wcerr<<L"test_string["<<i<<L"]="<<ict::test::test_wstring.at(i)<<std::endl;
                std::wcerr<<"c="<<c<<std::endl;
                out=2;
                break;
            }
        }
    }
    std::filesystem::remove_all(dirpath);
    return out;
}
REGISTER_TEST(single,tc3){
    int out=0;
    std::filesystem::remove_all(dirpath);
    std::filesystem::create_directory(dirpath);
    {
        ict::queue::single queue(dirpath);
        std::size_t max=1000000;
        std::string out;
        auto start=std::chrono::high_resolution_clock::now();
        for (std::size_t k=0;k<max;k++){
            std::size_t i=k%ict::test::test_string.size();
            queue.push(ict::test::test_string.at(i));
        }
        for (std::size_t k=0;k<max;k++){
            queue.pop(out);
        }
        auto elapsed=std::chrono::high_resolution_clock::now()-start;
        long long microseconds=std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        float rate=2*max*1000000;
        rate/=microseconds;
        std::cout<<"time("<<max<<" writes & "<<max<<" reads)="<<microseconds<<" microseconds"<<std::endl;
        std::cout<<"rate(writes & reads)="<<rate<<" operations/sec"<<std::endl;
    }
    std::filesystem::remove_all(dirpath);
    return out;
}
REGISTER_TEST(single,tc4){
    int out=0;
    std::filesystem::remove_all(dirpath);
    std::filesystem::create_directory(dirpath);
    {
        std::string input;
        ict::queue::single queue(dirpath);
        std::size_t max=7000;
        input.resize(128000,'x'); 
        auto start=std::chrono::high_resolution_clock::now();
        for (std::size_t k=0;k<max;k++){
            queue.push(input);
        }
        max=queue.size();
        auto elapsed=std::chrono::high_resolution_clock::now()-start;
        long long microseconds=std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        float rate=max*1000000;
        rate/=microseconds;
        std::cout<<"time("<<max<<" writes)="<<microseconds<<" microseconds"<<std::endl;
        std::cout<<"rate(writes)="<<rate<<" operations/sec"<<std::endl;
    }
    {
        std::string output;
        ict::queue::single queue(dirpath);
        std::size_t count=0;
        auto start=std::chrono::high_resolution_clock::now();
        while(!queue.empty()){
            queue.pop(output);
            count++;
        }
        auto elapsed=std::chrono::high_resolution_clock::now()-start;
        long long microseconds=std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        float rate=count*1000000;
        rate/=microseconds;
        std::cout<<"time("<<count<<" reads)="<<microseconds<<" microseconds"<<std::endl;
        std::cout<<"rate(reads)="<<rate<<" operations/sec"<<std::endl;
    }
    std::filesystem::remove_all(dirpath);
    return out;
}
#endif
//===========================================