// WindowsProject1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject1.h"
#include "Movie.h"
#include "Cinema.h"
#include "Screening.h"
#include <vector>
#include <string>
#include <fstream>
#include <CommCtrl.h>
#include <Commdlg.h>
#include <windowsx.h>

#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100

// Идентификаторы элементов управления
#define IDC_QUERY_COMBO 1001
#define IDC_RUN_BUTTON 1002
#define IDC_SAVE_BUTTON 1003
#define IDC_LISTVIEW 1004
#define IDC_INPUT_LABEL1 1005
#define IDC_INPUT_EDIT1 1006
#define IDC_INPUT_LABEL2 1007
#define IDC_INPUT_EDIT2 1008
#define IDC_INPUT_LABEL3 1009
#define IDC_INPUT_EDIT3 1010
#define IDC_INPUT_LABEL4 1011
#define IDC_INPUT_EDIT4 1012


// Глобальные переменные:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

// Глобальные переменные для UI и данных
HWND hWndQueryCombo;
HWND hWndRunButton;
HWND hWndSaveButton;
HWND hWndListView;
HWND hWndInputLabel1, hWndInputEdit1;
HWND hWndInputLabel2, hWndInputEdit2;
HWND hWndInputLabel3, hWndInputEdit3;
HWND hWndInputLabel4, hWndInputEdit4;

std::vector<Cinema> g_cinemas;
std::vector<Movie> g_movies;
std::vector<Screening> g_screenings;


// Прототипы функций
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void LoadSampleData();
void CreateUIComponents(HWND hWnd);
void HandleQuerySelection();
void RunSelectedQuery();
void SaveResultsToFile();
void DisplayCinemaRepertoire(const std::wstring& cinemaName);
void DisplayFreeSeats(const std::wstring& cinemaName, const std::wstring& movieTitle, const std::wstring& date, const std::wstring& time);
void DisplaySoldTickets(const std::wstring& movieTitle);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    LoadSampleData(); 

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        CreateUIComponents(hWnd);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        if (wmId == IDC_QUERY_COMBO && wmEvent == CBN_SELCHANGE)
        {
            HandleQuerySelection();
        }
        else if (wmId == IDC_RUN_BUTTON)
        {
            RunSelectedQuery();
        }
        else if (wmId == IDC_SAVE_BUTTON)
        {
            SaveResultsToFile();
        }

        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CreateUIComponents(HWND hWnd) {
    // Метка для выпадающего списка
    CreateWindowW(L"STATIC", L"Выберите запрос:", WS_VISIBLE | WS_CHILD,
        10, 10, 150, 20, hWnd, NULL, hInst, NULL);

    // Выпадающий список для выбора запроса
    hWndQueryCombo = CreateWindowW(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL,
        10, 30, 300, 200, hWnd, (HMENU)IDC_QUERY_COMBO, hInst, NULL);

    // Добавляем элементы в выпадающий список
    SendMessage(hWndQueryCombo, CB_ADDSTRING, 0, (LPARAM)L"Репертуар определенного кинотеатра");
    SendMessage(hWndQueryCombo, CB_ADDSTRING, 0, (LPARAM)L"Число свободных мест на сеанс");
    SendMessage(hWndQueryCombo, CB_ADDSTRING, 0, (LPARAM)L"Число проданных билетов на фильм");
    SendMessage(hWndQueryCombo, CB_SETCURSEL, 0, 0); // Выбираем первый элемент по умолчанию

    // Метки и поля для ввода
    hWndInputLabel1 = CreateWindowW(L"STATIC", L"Название кинотеатра:", WS_CHILD, 10, 60, 150, 20, hWnd, (HMENU)IDC_INPUT_LABEL1, hInst, NULL);
    hWndInputEdit1 = CreateWindowW(L"COMBOBOX", L"", CBS_DROPDOWN | WS_VSCROLL | WS_CHILD, 160, 60, 200, 150, hWnd, (HMENU)IDC_INPUT_EDIT1, hInst, NULL);
    for (const auto& cinema : g_cinemas) {
        SendMessage(hWndInputEdit1, CB_ADDSTRING, 0, (LPARAM)cinema.name.c_str());
    }

    hWndInputLabel2 = CreateWindowW(L"STATIC", L"Название фильма:", WS_CHILD, 10, 90, 150, 20, hWnd, (HMENU)IDC_INPUT_LABEL2, hInst, NULL);
    hWndInputEdit2 = CreateWindowW(L"COMBOBOX", L"", CBS_DROPDOWN | WS_VSCROLL | WS_CHILD, 160, 90, 200, 150, hWnd, (HMENU)IDC_INPUT_EDIT2, hInst, NULL);
    for (const auto& movie : g_movies) {
        SendMessage(hWndInputEdit2, CB_ADDSTRING, 0, (LPARAM)movie.title.c_str());
    }
    
    hWndInputLabel3 = CreateWindowW(L"STATIC", L"Дата (ГГГГ-ММ-ДД):", WS_CHILD, 10, 120, 150, 20, hWnd, (HMENU)IDC_INPUT_LABEL3, hInst, NULL);
    hWndInputEdit3 = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD, 160, 120, 200, 25, hWnd, (HMENU)IDC_INPUT_EDIT3, hInst, NULL);

    hWndInputLabel4 = CreateWindowW(L"STATIC", L"Время (ЧЧ:ММ):", WS_CHILD, 10, 150, 150, 20, hWnd, (HMENU)IDC_INPUT_LABEL4, hInst, NULL);
    hWndInputEdit4 = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD, 160, 150, 200, 25, hWnd, (HMENU)IDC_INPUT_EDIT4, hInst, NULL);

    // Кнопка "Выполнить"
    hWndRunButton = CreateWindowW(L"BUTTON", L"Выполнить", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 180, 100, 30, hWnd, (HMENU)IDC_RUN_BUTTON, hInst, NULL);

    // Кнопка "Сохранить"
    hWndSaveButton = CreateWindowW(L"BUTTON", L"Сохранить в файл", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        120, 180, 150, 30, hWnd, (HMENU)IDC_SAVE_BUTTON, hInst, NULL);


    // ListView для вывода результатов
    hWndListView = CreateWindowEx(0, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_EDITLABELS,
        10, 220, 760, 290, hWnd, (HMENU)IDC_LISTVIEW, hInst, NULL);
    
    ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // Изначально настроим видимость полей
    HandleQuerySelection();
}

void LoadSampleData() {
    // --- Фильмы ---
    g_movies.push_back({ L"Дюна: Часть вторая", L"Вильнёв", L"Дени", L"", L"Фрейзер", {L"Шаламе", L"Зендея"}, L"Фантастика", L"Warner Bros." });
    g_movies.push_back({ L"Оппенгеймер", L"Нолан", L"Кристофер", L"", L"Ван Хойтема", {L"Мёрфи", L"Блант"}, L"Биография", L"Universal Pictures" });
    g_movies.push_back({ L"Мастер и Маргарита", L"Локшин", L"Михаил", L"", L"Башта", {L"Цыганов", L"Снигирь"}, L"Драма", L"Марс Медиа" });

    // --- Кинотеатры ---
    g_cinemas.push_back({ L"Синема Парк", L"Высшая", 500, 8, L"Открыт" });
    g_cinemas.push_back({ L"Киномакс", L"Первая", 400, 6, L"Открыт" });
    g_cinemas.push_back({ L"Аврора", L"Вторая", 250, 4, L"Ремонт" });

    // --- Сеансы ---
    // Синема Парк
    g_screenings.push_back({ 0, 0, L"2024-07-20", L"18:00", 450.0, 100, 25 });
    g_screenings.push_back({ 0, 0, L"2024-07-20", L"21:00", 500.0, 100, 40 });
    g_screenings.push_back({ 0, 1, L"2024-07-20", L"19:00", 480.0, 120, 10 });
    // Киномакс
    g_screenings.push_back({ 1, 0, L"2024-07-20", L"17:30", 400.0, 80, 5 });
    g_screenings.push_back({ 1, 2, L"2024-07-20", L"20:00", 420.0, 90, 30 });
    g_screenings.push_back({ 1, 2, L"2024-07-21", L"20:00", 420.0, 90, 85 });
}

void HandleQuerySelection() {
    int selection = SendMessage(hWndQueryCombo, CB_GETCURSEL, 0, 0);

    // Сначала скроем все поля
    ShowWindow(hWndInputLabel1, SW_HIDE);
    ShowWindow(hWndInputEdit1, SW_HIDE);
    ShowWindow(hWndInputLabel2, SW_HIDE);
    ShowWindow(hWndInputEdit2, SW_HIDE);
    ShowWindow(hWndInputLabel3, SW_HIDE);
    ShowWindow(hWndInputEdit3, SW_HIDE);
    ShowWindow(hWndInputLabel4, SW_HIDE);
    ShowWindow(hWndInputEdit4, SW_HIDE);

    // Сброс позиций
    SetWindowPos(hWndInputLabel1, NULL, 10, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(hWndInputEdit1, NULL, 160, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(hWndInputLabel2, NULL, 10, 90, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(hWndInputEdit2, NULL, 160, 90, 0, 0, SWP_NOSIZE | SWP_NOZORDER);


    switch (selection)
    {
    case 0: // Репертуар кинотеатра
        ShowWindow(hWndInputLabel1, SW_SHOW);
        ShowWindow(hWndInputEdit1, SW_SHOW);
        break;
    case 1: // Свободные места
        ShowWindow(hWndInputLabel1, SW_SHOW);
        ShowWindow(hWndInputEdit1, SW_SHOW);
        ShowWindow(hWndInputLabel2, SW_SHOW);
        ShowWindow(hWndInputEdit2, SW_SHOW);
        ShowWindow(hWndInputLabel3, SW_SHOW);
        ShowWindow(hWndInputEdit3, SW_SHOW);
        ShowWindow(hWndInputLabel4, SW_SHOW);
        ShowWindow(hWndInputEdit4, SW_SHOW);
        break;
    case 2: // Проданные билеты
        // Перемещаем второе поле на место первого
        SetWindowPos(hWndInputLabel2, NULL, 10, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(hWndInputEdit2, NULL, 160, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        ShowWindow(hWndInputLabel2, SW_SHOW);
        ShowWindow(hWndInputEdit2, SW_SHOW);
        break;
    }
}

void RunSelectedQuery() {
    int selection = SendMessage(hWndQueryCombo, CB_GETCURSEL, 0, 0);

    wchar_t buffer1[256];
    wchar_t buffer2[256];
    wchar_t buffer3[256];
    wchar_t buffer4[256];

    GetWindowTextW(hWndInputEdit1, buffer1, 256);
    GetWindowTextW(hWndInputEdit2, buffer2, 256);
    GetWindowTextW(hWndInputEdit3, buffer3, 256);
    GetWindowTextW(hWndInputEdit4, buffer4, 256);

    std::wstring input1 = buffer1;
    std::wstring input2 = buffer2;
    std::wstring input3 = buffer3;
    std::wstring input4 = buffer4;

    switch (selection)
    {
    case 0: // Репертуар кинотеатра
        DisplayCinemaRepertoire(input1);
        break;
    case 1: // Свободные места
        DisplayFreeSeats(input1, input2, input3, input4);
        break;
    case 2: // Проданные билеты
        DisplaySoldTickets(input2); // Используем input2 для фильмов
        break;
    }
}

void SaveResultsToFile() {
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetParent(hWndListView);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"txt";

    if (GetSaveFileNameW(&ofn) == TRUE)
    {
        std::wofstream outFile(ofn.lpstrFile);
        if (!outFile.is_open()) {
            MessageBoxW(GetParent(hWndListView), L"Не удалось создать файл.", L"Ошибка", MB_OK | MB_ICONERROR);
            return;
        }
        
        outFile.imbue(std::locale(""));

        HWND hHeader = ListView_GetHeader(hWndListView);
        int colCount = Header_GetItemCount(hHeader);
        wchar_t colText[256];
        HDITEMW hdi;
        hdi.mask = HDI_TEXT;
        hdi.pszText = colText;
        hdi.cchTextMax = 256;

        for (int i = 0; i < colCount; i++) {
            Header_GetItem(hHeader, i, &hdi);
            outFile << colText << (i == colCount - 1 ? L"" : L"\t\t");
        }
        outFile << std::endl;

        int rowCount = ListView_GetItemCount(hWndListView);
        wchar_t itemText[256];
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                ListView_GetItemText(hWndListView, i, j, itemText, 256);
                outFile << itemText << (j == colCount - 1 ? L"" : L"\t\t");
            }
            outFile << std::endl;
        }
        outFile.close();
    }
}

void DisplayCinemaRepertoire(const std::wstring& cinemaName) {
    ListView_DeleteAllItems(hWndListView);
    while (ListView_DeleteColumn(hWndListView, 0)); // Clear columns

    // Find cinema id
    int cinemaId = -1;
    for (size_t i = 0; i < g_cinemas.size(); ++i) {
        if (g_cinemas[i].name == cinemaName) {
            cinemaId = i;
            break;
        }
    }

    if (cinemaId == -1) {
        MessageBoxW(GetParent(hWndListView), L"Кинотеатр не найден.", L"Информация", MB_OK | MB_ICONINFORMATION);
        return;
    }

    // Setup columns
    LVCOLUMNW lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    const wchar_t* headers[] = { L"Фильм", L"Дата", L"Время", L"Цена", L"Свободно мест" };
    int widths[] = { 200, 100, 100, 100, 120 };
    for (int i = 0; i < 5; i++) {
        lvc.iSubItem = i;
        lvc.cx = widths[i];
        lvc.pszText = (LPWSTR)headers[i];
        ListView_InsertColumn(hWndListView, i, &lvc);
    }

    // Populate list
    int itemIndex = 0;
    for (const auto& screening : g_screenings) {
        if (screening.cinema_id == cinemaId) {
            LVITEMW lvi;
            lvi.mask = LVIF_TEXT;
            lvi.iItem = itemIndex;
            lvi.iSubItem = 0;
            lvi.pszText = (LPWSTR)g_movies[screening.movie_id].title.c_str();
            ListView_InsertItem(hWndListView, &lvi);

            ListView_SetItemText(hWndListView, itemIndex, 1, (LPWSTR)screening.date.c_str());
            ListView_SetItemText(hWndListView, itemIndex, 2, (LPWSTR)screening.time.c_str());
            std::wstring priceStr = std::to_wstring(screening.price);
            ListView_SetItemText(hWndListView, itemIndex, 3, (LPWSTR)priceStr.c_str());
            std::wstring freeSeatsStr = std::to_wstring(screening.free_seats);
            ListView_SetItemText(hWndListView, itemIndex, 4, (LPWSTR)freeSeatsStr.c_str());

            itemIndex++;
        }
    }
}

void DisplayFreeSeats(const std::wstring& cinemaName, const std::wstring& movieTitle, const std::wstring& date, const std::wstring& time) {
    ListView_DeleteAllItems(hWndListView);
    while (ListView_DeleteColumn(hWndListView, 0));

    int cinemaId = -1, movieId = -1;
    for (size_t i = 0; i < g_cinemas.size(); ++i) if (g_cinemas[i].name == cinemaName) cinemaId = i;
    for (size_t i = 0; i < g_movies.size(); ++i) if (g_movies[i].title == movieTitle) movieId = i;
    
    if (cinemaId == -1 || movieId == -1) {
        MessageBoxW(GetParent(hWndListView), L"Кинотеатр или фильм не найден.", L"Информация", MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    LVCOLUMNW lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    const wchar_t* headers[] = { L"Кинотеатр", L"Фильм", L"Дата", L"Время", L"Свободно мест" };
    int widths[] = { 150, 200, 100, 100, 120 };
    for (int i = 0; i < 5; i++) {
        lvc.iSubItem = i;
        lvc.cx = widths[i];
        lvc.pszText = (LPWSTR)headers[i];
        ListView_InsertColumn(hWndListView, i, &lvc);
    }

    int itemIndex = 0;
    for (const auto& screening : g_screenings) {
        if (screening.cinema_id == cinemaId && screening.movie_id == movieId && screening.date == date && screening.time == time) {
            LVITEMW lvi;
            lvi.mask = LVIF_TEXT;
            lvi.iItem = itemIndex;
            lvi.iSubItem = 0;
            lvi.pszText = (LPWSTR)g_cinemas[screening.cinema_id].name.c_str();
            ListView_InsertItem(hWndListView, &lvi);

            ListView_SetItemText(hWndListView, itemIndex, 1, (LPWSTR)g_movies[screening.movie_id].title.c_str());
            ListView_SetItemText(hWndListView, itemIndex, 2, (LPWSTR)screening.date.c_str());
            ListView_SetItemText(hWndListView, itemIndex, 3, (LPWSTR)screening.time.c_str());
            std::wstring freeSeatsStr = std::to_wstring(screening.free_seats);
            ListView_SetItemText(hWndListView, itemIndex, 4, (LPWSTR)freeSeatsStr.c_str());

            itemIndex++;
        }
    }
    if (itemIndex == 0) {
        MessageBoxW(GetParent(hWndListView), L"Сеанс с указанными параметрами не найден.", L"Информация", MB_OK | MB_ICONINFORMATION);
    }
}

void DisplaySoldTickets(const std::wstring& movieTitle) {
    ListView_DeleteAllItems(hWndListView);
    while (ListView_DeleteColumn(hWndListView, 0));

    int movieId = -1;
    for (size_t i = 0; i < g_movies.size(); ++i) if (g_movies[i].title == movieTitle) movieId = i;

    if (movieId == -1) {
        MessageBoxW(GetParent(hWndListView), L"Фильм не найден.", L"Информация", MB_OK | MB_ICONINFORMATION);
        return;
    }

    int totalSold = 0;
    for (const auto& screening : g_screenings) {
        if (screening.movie_id == movieId) {
            totalSold += (screening.total_seats - screening.free_seats);
        }
    }

    LVCOLUMNW lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    const wchar_t* headers[] = { L"Фильм", L"Всего продано билетов" };
    int widths[] = { 250, 200 };
    for (int i = 0; i < 2; i++) {
        lvc.iSubItem = i;
        lvc.cx = widths[i];
        lvc.pszText = (LPWSTR)headers[i];
        ListView_InsertColumn(hWndListView, i, &lvc);
    }

    LVITEMW lvi;
    lvi.mask = LVIF_TEXT;
    lvi.iItem = 0;
    lvi.iSubItem = 0;
    lvi.pszText = (LPWSTR)g_movies[movieId].title.c_str();
    ListView_InsertItem(hWndListView, &lvi);
    
    std::wstring soldStr = std::to_wstring(totalSold);
    ListView_SetItemText(hWndListView, 0, 1, (LPWSTR)soldStr.c_str());
}


// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
