// HiMail.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <limits>

void SetConsoleColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

void PrintLogo() {
    SetConsoleColor(10, 0);
    std::cout << R"(

 __  __     __     ______   ______     ______     ______     ______     ______     ______    
/\ \_\ \   /\ \   /\  == \ /\  == \   /\  __ \   /\  ___\   /\  ___\   /\  ___\   /\  ___\   
\ \  __ \  \ \ \  \ \  _-/ \ \  __<   \ \ \/\ \  \ \ \____  \ \  __\   \ \___  \  \ \___  \  
 \ \_\ \_\  \ \_\  \ \_\    \ \_\ \_\  \ \_____\  \ \_____\  \ \_____\  \/\_____\  \/\_____\ 
  \/_/\/_/   \/_/   \/_/     \/_/ /_/   \/_____/   \/_____/   \/_____/   \/_____/   \/_____/ 
                                                                                             
)" << std::endl;
    SetConsoleColor(7, 0);

    std::this_thread::sleep_for(std::chrono::seconds(1));

}

void ShowMenu() {



}

DWORD GetProcessIdByName(const std::wstring& processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    DWORD processId = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry)) {
        do {
            if (processName == entry.szExeFile) {
                processId = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return processId;
}




int Action1() {

    std::wstring processName;
    std::wcout << L"Enter name of process you want to kill: ";
    std::getline(std::wcin, processName);

    DWORD processId = GetProcessIdByName(processName);

    if (processId == 0) {
        std::wcerr << L"Process " << processName << L" was not found" << std::endl;
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::cerr << "Failed to open process." << std::endl;
        return 1;
    }

    LPVOID pRemoteCode = VirtualAllocEx(hProcess, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!pRemoteCode) {
        std::cerr << "Failed to allocate memory." << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    unsigned char shellcode[] = {
        0x90,
        0x90,
        0xCC
    };

    SIZE_T writtenBytes;
    if (!WriteProcessMemory(hProcess, pRemoteCode, shellcode, sizeof(shellcode), &writtenBytes)) {
        std::cerr << "Failed to write code to memory." << std::endl;
        VirtualFreeEx(hProcess, pRemoteCode, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteCode, NULL, 0, NULL);
    if (!hThread) {
        std::cerr << "Failed to create thread." << std::endl;
        VirtualFreeEx(hProcess, pRemoteCode, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, pRemoteCode, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    std::cout << "Done" << std::endl;
    return 0;


}


int main() {


    system("CLS");

    PrintLogo();

    int choice = 0;

    std::cout << "\nChoose from these options:" << std::endl;
    std::cout << "1) kill process" << std::endl;
    std::cout << "2) listen to process" << std::endl;
    std::cout << "3) exit" << std::endl;
    std::cout << "number: ";


    std::cin >> choice;
    std::cin.clear();
    std::cin.ignore(10000, '\n');




    switch (choice) {
    case 1:
        Action1();
        break;
    case 2:

        break;
    case 3:
        std::cout << "Ending program." << std::endl;
        break;
    default:
        std::cout << "false, try again." << std::endl;
    }
    //test

    return 0;

}





// Spuštění programu: Ctrl+F5 nebo nabídka Ladit > Spustit bez ladění
// Ladění programu: F5 nebo nabídka Ladit > Spustit ladění

// Tipy pro zahájení práce:
//   1. K přidání nebo správě souborů použijte okno Průzkumník řešení.
//   2. Pro připojení ke správě zdrojového kódu použijte okno Team Explorer.
//   3. K zobrazení výstupu sestavení a dalších zpráv použijte okno Výstup.
//   4. K zobrazení chyb použijte okno Seznam chyb.
//   5. Pokud chcete vytvořit nové soubory kódu, přejděte na Projekt > Přidat novou položku. Pokud chcete přidat do projektu existující soubory kódu, přejděte na Projekt > Přidat existující položku.
//   6. Pokud budete chtít v budoucnu znovu otevřít tento projekt, přejděte na Soubor > Otevřít > Projekt a vyberte příslušný soubor .sln.
