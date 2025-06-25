#pragma once

#include <string>
#include <vector>

// Класс для представления фильма
class Movie {
public:
    std::wstring title;
    std::wstring director_surname;
    std::wstring director_firstname;
    std::wstring director_patronymic;
    std::wstring cameraman;
    std::vector<std::wstring> actors;
    std::wstring genre;
    std::wstring studio;
}; 