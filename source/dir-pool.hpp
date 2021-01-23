//! @file
//! @brief Directory pool module - header file.
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
#ifndef _DIR_POOL_HEADER
#define _DIR_POOL_HEADER
//============================================
#include <string>
#include <sstream>
#include <set>
#include "types.hpp"
//============================================
namespace ict { namespace  queue { namespace  dir {
//===========================================
//! Pula katalogów
class pool{
private:
    //! Ścieżka do katalogu z kolekami.
    const ict::queue::types::path_t dir;
    //! Lista katalogów.
    std::set<std::string> ids;
    //! 
    //! @brief Koduje string w taki sposób, by można go było użyć w nazwie katalogu.
    //! 
    //! @param input String do zakodowania.
    //! @return Zakodowany string. 
    //! 
    static std::string encodeStringForPath(const std::string & input);
    //! 
    //! @brief Odkodowuje string z nazwy katalogu.
    //! 
    //! @param input Nazwa katalogu do odkodowania.
    //! @return Odkodowany string.
    //! 
    static std::string decodeStringForPath(const std::string & input);
    //! 
    //! @brief Zamienia ID na string, który można użyć do nazwy katalogu.
    //! 
    //! @param input ID w postaci string.
    //! @param output String wyjściowy.
    //! 
    static void idToString(const std::string & input,std::string & output);
    //! 
    //! @brief Zamienia string z nazwy katalogu na ID.
    //! 
    //! @param input String wejściowy.
    //! @param output ID w postaci string.
    //! 
    static void idFromString(const std::string & input,std::string & output);
    //! 
    //! @brief Zamienia ID na string, który można użyć do nazwy katalogu.
    //! 
    //! @param input ID w postaci numeru.
    //! @param output String wyjściowy.
    //! 
    template <typename Identifier> static void idToString(const Identifier & input,std::string & output){
        std::stringstream stream;
        stream<<input;
        output=stream.str();
    }
    //! 
    //! @brief Zamienia string z nazwy katalogu na ID.
    //! 
    //! @param input String wejściowy.
    //! @param output ID w postaci numeru.
    //! 
    template <typename Identifier> static void idFromString(const std::string & input,Identifier & output){
        std::stringstream stream;
        stream<<input;
        stream>>output;
    }
    ict::queue::types::path_t getPathString(const std::string & id) const;
    //! 
    //! @brief Zwraca wszystkie ID w postaci string.
    //! 
    //! @param output Wszystkie ID w postaci string.
    //! 
    void getAllIdsString(std::set<std::string> & output);
    //! 
    //! @brief Dodaje kolejkę do puli.
    //! 
    //! @param id ID nowej kolejki.
    //! 
    void addString(const std::string & id);
    //! 
    //! @brief Usuwa kolejkę z puli.
    //! 
    //! @param id ID kolejki do usunięcia.
    //! 
    void removeString(const std::string & id);
    //! 
    //! @brief Sprawdza, czy istnieje kolejka o podanym ID.
    //! 
    //! @param id ID kolejki.
    //! 
    bool existsString(const std::string & id);
public:
    //! 
    //! @brief Konstruktor puli katalogów.
    //! 
    //! @param dirname Ścieżka do katalogu z pulą katalogów.
    //! 
    pool(const ict::queue::types::path_t & dirname);
    //! 
    //! @brief Zwraca ścieżkę do katalogu o podanym id.
    //! 
    //! @param id Identyfikator katalogu.
    //! @return Ścieżka do pliku.
    //! 
    template <typename Identifier> const ict::queue::types::path_t getPath(const Identifier & id) const{
        std::string s;
        idToString(id,s);
        return(getPathString(s));
    }
    //! 
    //! @brief Zwraca listę identyfikatorów kolejek.
    //! 
    //! @tparam Identifier Typ identyfikatora.
    //! @param output Aktualna lista identyfikatorów.
    //! 
    template <typename Identifier> void getAllIds(std::set<Identifier> & output){
        output.clear();
        if (ids.empty()) getAllIdsString(ids);
        for (const std::string & s:ids) {
            Identifier i;
            idFromString(s,i);
            output.emplace(i);
        };
    }
    //! 
    //! @brief Dodaje kolejkę do puli.
    //! 
    //! @tparam Identifier Typ identyfikatora.
    //! @param id ID nowej kolejki.
    //! 
    template <typename Identifier> void add(const Identifier & id){
        std::string s;
        idToString(id,s);
        addString(s);
    }
    //! 
    //! @brief Usuwa kolejkę z puli.
    //! 
    //! @tparam Identifier Identifier Typ identyfikatora.
    //! @param id ID kolejki do usunięcia.
    //! 
    template <typename Identifier> void remove(const Identifier & id){
        std::string s;
        idToString(id,s);
        removeString(s);
    }
    //! 
    //! @brief Sprawdza, czy istnieje kolejka o podanym ID.
    //! 
    //! @tparam Identifier Identifier Typ identyfikatora.
    //! @param id ID kolejki.
    //! 
    template <typename Identifier> bool exists(const Identifier & id){
        std::string s;
        idToString(id,s);
        return(existsString(s));
    }
    //! 
    //! @brief Zwraca aktualny rozmiar puli katalogów.
    //! 
    //! @return Rozmiar puli katalogów.
    //! 
    std::size_t size();
    //! 
    //! @brief Sprawdza, czy pula katalogów jest pusta.
    //! 
    //! @return true Jest pusta.
    //! @return false Nie jest pusta.
    //! 
    bool empty();
    //! 
    //! @brief Usuwa wszystkie katalogi z puli.
    //! 
    void clear();
};
//===========================================
} } }
//============================================
#endif