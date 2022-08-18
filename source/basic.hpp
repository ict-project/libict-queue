//! @file
//! @brief Basic queue module - header file.
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
#ifndef _BASIC_QUEUE_HEADER
#define _BASIC_QUEUE_HEADER
//============================================
#include "types.hpp"
#include "file-interface.hpp"
#include <mutex>
//============================================
namespace ict { namespace  queue { 
//===========================================
class basic {
private:
    //! Maksymalny rozmiar pliku, po przekroczeniu którego tworzony jest nowy plik.
    const std::size_t max_file_size;
    //! Interfejs do puli plików.
    ict::queue::file::interface iface;
    //! Mutex dla zapisu.
    std::mutex writeMutex;
    //! Mutex dla odczytu.
    std::mutex readMutex;
    //! Rekord do zapisania w pliku (informacja zapisie do kolejki).
    ict::queue::types::record_t writeRecord={ict::queue::types::payload_size_record,0};
    //! Rekord do zapisania w pliku (informacja odczycie z kolejki).
    ict::queue::types::record_t readRecord={ict::queue::types::read_confirm_record,0};
    //! Flaga zapisu.
    bool writeOperation=false;
    //! Flaga odczytu.
    bool readOperation=false;
public:
    //! 
    //! @brief Konstruktor kolejki.
    //! 
    //! @param dirname Ścieżka do katalogu z plikami.
    //! @param maxFileSize Maksymalny rozmiar pliku, po przekroczeniu którego tworzony jest nowy plik.
    //! @param maxFiles Maksymalna liczba plików w puli.
    //! 
    basic(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff);
    //! 
    //! @brief Zapisuje informację o rozmiarze danych.
    //! 
    //! @param size Rozmiar danych do zapisania w kolejce.
    //! 
     void writeSize(const std::size_t & size);
    //! 
    //! @brief Zapisuje dane do kolejki. Wcześniej trzeba wykonać writeSize(), by wskazać rozmiar tych danych.
    //! 
    //! @param content Wskażnik do odczytu danych.
    //! 
    void writeContent(const char * content);
    //! 
    //! @brief Odczytuje informację o rozmiarze danych.
    //! 
    //! @param size Rozmiar danych do odczytania z kolejki.
    //!
    void readSize(std::size_t & size);
    //! 
    //! @brief Odczytuje dane z kolejki. Wcześniej trzeba wykonać readSize(), by poznać rozmiar tych danych.
    //! 
    //! @param content Wskażnik do zapisu danych.
    //! 
    void readContent(char * content);
    //! 
    //! @brief Zwraca aktualny rozmiar kolejki.
    //! 
    //! @return Rozmiar kolejki.
    //! 
    std::size_t size();
    //! 
    //! @brief Sprawdza, czy kolejka jest pusta.
    //! 
    //! @return true Jest pusta.
    //! @return false Nie jest pusta.
    //! 
    bool empty();
    //! 
    //! @brief Czyści kolejkę.
    //! 
    void clear();
    //! 
    //! @brief Sprawdza, czy interfejs wymaga przeładowania i przeładowuje, jeśli jest to potrzebne. 
    //! 
    bool refresh();
};
//===========================================
} }
//============================================
#endif