//! @file
//! @brief File pool module - header file.
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
#ifndef _FILE_POOL_HEADER
#define _FILE_POOL_HEADER
//============================================
#include <string>
#include <vector>
#include "types.hpp"
//============================================
namespace ict { namespace  queue { namespace  file {
//===========================================
//! Pola plików, w których kolejka zapisuje dane.
class pool{
private:
    //! Typ - numer pliku.
    typedef uint64_t number_t;
    //! Typ - Metadane pliku.
    struct item_t {
        ict::queue::types::path_t path;
        number_t number;
    };
    //! Typ - lista plików.
    typedef std::vector<item_t> list_t;
    //! Ścieżka do katalogu z plikami.
    const ict::queue::types::path_t dir;
    //! Maksymalna liczba plików.
    const std::size_t max_files;
    //! Maksymalny rozmiar pliku.
    const std::size_t max_file_size;
    //! Lista plików.
    list_t list;
    //! 
    //! @brief Zwraca ścieżkę do pliku o podanum numerze.
    //! 
    //! @param n Numer pliku.
    //! @return Ścieżka do pliku.
    //! 
    ict::queue::types::path_t getPathString(number_t n) const;
    //! 
    //! @brief Zwraca wyrażenie regularne do nazw plików.
    //! 
    //! @return Wurażenie regularne.
    //! 
    std::string getRegexString() const;
    //! 
    //! @brief Ładuje listę plików ze wskazanego katalogu.
    //! 
    void loadFileList();
    //! 
    //! @brief Tworzy nowy plik.
    //! 
    //! @param path Ścieżka do pliku.
    //! 
    void createFile(const ict::queue::types::path_t & path) const;
    //! 
    //! @brief Usuwa istniejący plik.
    //! 
    //! @param path Ścieżka do pliku.
    //! 
    void removeFile(const ict::queue::types::path_t & path) const;
public:
    //! 
    //! @brief Konstruktor puli plików.
    //! 
    //! @param dirname Ścieżka do katalogu z plikami.
    //! @param max Maksymalna liczba plików.
    //! 
    pool(const ict::queue::types::path_t & dirname,const std::size_t & maxFileSize=1000000,const std::size_t & maxFiles=0xffffffff);
    //! 
    //! @brief Zwraca ścieżkę do pliku o podanym indeksie.
    //! 
    //! @param index Indeks pliku.
    //! @return Ścieżka do pliku. 
    //! 
    const ict::queue::types::path_t & getPath(const std::size_t & index=0) const;
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
    //! @brief Dodaje nowy plik do puli (na początku).
    //! 
    void pushFront();
    //! 
    //! @brief Usuwa plik z puli (na końcu).
    //!  
    void popBack();
    //! 
    //! @brief Usuwa wszystkie pliki z puli.
    //! 
    void clear();
    //! 
    //! @brief Sprawdza, czy pula wymaga przeładowania i przeładowuje, jeśli jest tp potrzebne. 
    //!
    bool refresh();
};
//===========================================
} } }
//============================================
#endif