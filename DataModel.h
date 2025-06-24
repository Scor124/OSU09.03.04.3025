#pragma once

#include <string>
#include <vector>
#include <list>

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

// Класс для представления кинотеатра
class Cinema {
public:
    std::wstring name;
    std::wstring category;
    int capacity;
    int num_halls;
    std::wstring state;
};

// Класс для представления сеанса
class Screening {
public:
    int cinema_id; // ID кинотеатра
    int movie_id;  // ID фильма
    std::wstring date;
    std::wstring time;
    double price;
    int total_seats;
    int free_seats;
}; 