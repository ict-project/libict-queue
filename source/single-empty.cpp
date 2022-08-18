#include <iostream>
#include "single.hpp"
//=================================================
int main(int argc,const char **argv){
    if (argc<2) {
        std::cerr<<"Queue path not provided!"<<std::endl;
        return -1;
    } else try {
        ict::queue::single q(argv[1]);
        return q.empty()?0:1;
    } catch (const std::exception & e){
        std::cerr<<e.what()<<std::endl;
        return -2;
    }
    return 0;
}
//===========================================