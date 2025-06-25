#pragma once

#include <string>

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