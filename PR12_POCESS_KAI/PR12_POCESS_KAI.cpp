// PR12_POCESS_KAI.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <string>
#include <limits>

using namespace std;

const int MAX_PROCESSOV = 20;

struct MoiProcess {
    string imya;
    DWORD pid;
    HANDLE hendl;
};

MoiProcess processy[MAX_PROCESSOV];
int skolko_zapusheno = 0;

bool proverit_rabotaet_li(DWORD pid) {
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (h == NULL) return false;

    DWORD kod_vihoda;
    GetExitCodeProcess(h, &kod_vihoda);
    CloseHandle(h);

    return (kod_vihoda == 259);
}

void zapustit_programmu(string imya_faila, string argumenty, string dlya_otobrazheniya) {
    if (skolko_zapusheno >= MAX_PROCESSOV) {
        cout << "[ОШИБКА] Лимит 20 процессов!" << endl;
        return;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    string polnaya_stroka = imya_faila + " " + argumenty;
    char bufer[1024];
    strcpy(bufer, polnaya_stroka.c_str());

    if (!CreateProcessA(NULL, bufer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "[ОШИБКА] Не смог запустить! Код: " << GetLastError() << endl;
        return;
    }

    processy[skolko_zapusheno].imya = dlya_otobrazheniya;
    processy[skolko_zapusheno].pid = pi.dwProcessId;
    processy[skolko_zapusheno].hendl = pi.hProcess;

    cout << "[УСПЕХ] Запущено: " << dlya_otobrazheniya << " (PID: " << pi.dwProcessId << ")" << endl;

    skolko_zapusheno++;
    CloseHandle(pi.hThread);
}

void pokazat_spisok() {
    cout << "\n--- СПИСОК ПРОЦЕССОВ ---" << endl;
    if (skolko_zapusheno == 0) {
        cout << "Пусто. Ничего не запускали." << endl;
        return;
    }

    int rabotaet = 0;

    for (int i = 0; i < skolko_zapusheno; i++) {
        bool zhivoy = proverit_rabotaet_li(processy[i].pid);
        string status = zhivoy ? "РАБОТАЕТ" : "ЗАВЕРШЕН";
        if (zhivoy) rabotaet++;

        cout << i + 1 << ". " << processy[i].imya
            << " | PID: " << processy[i].pid
            << " | " << status << endl;
    }
    cout << "---------------------------------" << endl;
    cout << "Всего: " << skolko_zapusheno << " | Работает: " << rabotaet << endl;
}

void ubit_process() {
    pokazat_spisok();
    if (skolko_zapusheno == 0) return;

    cout << "Введи номер для убийства (1-" << skolko_zapusheno << "): ";
    int nomer;
    cin >> nomer;

    if (nomer < 1 || nomer > skolko_zapusheno) {
        cout << "[ОШИБКА] Нет такого номера!" << endl;
        return;
    }

    int index = nomer - 1;

    if (!proverit_rabotaet_li(processy[index].pid)) {
        cout << "Он уже завершен." << endl;
        return;
    }

    if (TerminateProcess(processy[index].hendl, 1)) {
        cout << "[УСПЕХ] Убит: " << processy[index].imya << endl;
    }
    else {
        cout << "[ОШИБКА] Не смог убить. Код: " << GetLastError() << endl;
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int punkt_menyu = -1;

    while (punkt_menyu != 0) {
        system("cls");

        cout << "========== МЕНЕДЖЕР ПРОЦЕССОВ ==========" << endl;
        cout << "1. Запустить Диспетчер задач (taskmgr)" << endl;
        cout << "2. Запустить Пинг (ping 8.8.8.8)" << endl;
        cout << "3. Запустить Пинг (-n 4)" << endl;
        cout << "4. Запустить Пинг (-t)" << endl;
        cout << "5. Запустить Вариант 5 (Делители)" << endl;
        cout << "6. Показать список процессов" << endl;
        cout << "7. Принудительно убить процесс" << endl;
        cout << "0. Выход" << endl;
        cout << "Твой выбор: ";

        if (!(cin >> punkt_menyu)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка! Введи цифру от 0 до 7." << endl;
            system("pause");
            continue;
        }

        if (punkt_menyu == 1) {
            zapustit_programmu("taskmgr.exe", "", "Диспетчер задач");
            system("pause");
        }
        else if (punkt_menyu == 2) {
            zapustit_programmu("ping.exe", "8.8.8.8", "Ping обычный");
            system("pause");
        }
        else if (punkt_menyu == 3) {
            zapustit_programmu("ping.exe", "-n 4 8.8.8.8", "Ping 4 пакета");
            system("pause");
        }
        else if (punkt_menyu == 4) {
            zapustit_programmu("ping.exe", "-t 8.8.8.8", "Ping бесконечный");
            system("pause");
        }
        else if (punkt_menyu == 5) {
            int a, b;
            cout << "Начало диапазона: ";
            cin >> a;
            cout << "Конец диапазона: ";
            cin >> b;

            string argumenty_dlya_5 = to_string(a) + " " + to_string(b);
            zapustit_programmu("PR12_PROCESS_V5.exe", argumenty_dlya_5, "Мой Вариант 5");
            system("pause");
        }
        else if (punkt_menyu == 6) {
            pokazat_spisok();
            system("pause");
        }
        else if (punkt_menyu == 7) {
            ubit_process();
            system("pause");
        }
        else if (punkt_menyu == 0) {
            cout << "Выходим..." << endl;
        }
        else {
            cout << "Не та цифра. Введи от 0 до 7." << endl;
            system("pause");
        }
    }

    for (int i = 0; i < skolko_zapusheno; i++) {
        CloseHandle(processy[i].hendl);
    }

    return 0;
}