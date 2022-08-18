//! @file
//! @brief File interface module - header file.
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
#ifndef _FILE_INTERFACE_HEADER
#define _FILE_INTERFACE_HEADER
//============================================
#include "types.hpp"
#include "file-pool.hpp"
#include <fstream>
#include <memory>
#include <atomic>
//============================================
namespace ict { namespace  queue { namespace  file {
//===========================================
class interface {
private:
    //! Informacja, czy rozmiar kolejki został zainicjowany.
    bool readySize=false;
    //! Aktualny rozmiar kolejki.
    std::atomic_size_t queue_size;
    //! Pula plików.
    pool fpool;
    //! Plik (strumień) do zapisu.
    std::unique_ptr<std::ofstream> ostream;
    //! Plik (strumień) do odczytu.
    std::unique_ptr<std::ifstream> istream;
    //! 
    //! @brief Zapisuje metadane w pliku.
    //! 
    //! @param currentSize Aktualny rozmiar kolejki.
    //! 
    void writeInfo();
    //! 
    //! @brief Odczytuje z pliku jaka powinna być pozycja odczytu.
    //! 
    //! @return Pozycja odczytu.
    //! 
    std::size_t getPositionFromFile();
    //! 
    //! @brief Odczytuje z pliku jaki powinien być rozmiar kolejki.
    //! 
    //! @return Rozmiar kolejki.
    //! 
    std::size_t getSizeFromFile();
public:
    //! 
    //! @brief Konstruktor interfejsu plików.
    //! 
    //! @param dirname Ścieżka do katalogu z plikami.
    //! @param max Maksymalna liczba plików w puli.
    //! 
    interface(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff);
    //! 
    //! @brief Zwraca plik (strumień) do zapisu.
    //! 
    //! @return Plik (strumień) do zapisu.
    //! 
    std::ofstream & getWriteStream();
    //! 
    //! @brief Zwraca plik (strumień) do odczytu.
    //! 
    //! @return Plik (strumień) do odczytu.
    //! 
    std::ifstream & getReadStream();
    //! 
    //! @brief Przechodzi do kolejnego pliku do zapisu.
    //! 
    //! @param currentSize Aktualny rozmiar kolejki.
    //! 
    void nextWriteStream();
    //! 
    //! @brief Przechodzi do kolejnego pliku do odczytu.
    //! 
    //! @param currentSize Aktualny rozmiar kolejki.
    //! 
    void nextReadStream();
    //! 
    //! @brief Zwraca aktualny rozmiar puli plików.
    //! 
    //! @return Rozmiar puli plików.
    //! 
    std::size_t size() const;
    //! 
    //! @brief Sprawdza, czy pula plików jest pusta.
    //! 
    //! @return true Jest pusta.
    //! @return false Nie jest pusta.
    //! 
    bool empty() const;
    //! 
    //! @brief Usuwa wszystkie pliki z puli.
    //! 
    void clear();
    //! 
    //! @brief Udostępnia aktualny rozmiar kolejki.
    //! 
    //! @return Aktualny rozmiar kolejki. 
    //! 
    std::atomic_size_t & queueSize();
    //! 
    //! @brief Sprawdza, czy interfejs wymaga przeładowania i przeładowuje, jeśli jest to potrzebne. 
    //! 
    bool refresh();
};
//===========================================
} } }
//============================================
#endif