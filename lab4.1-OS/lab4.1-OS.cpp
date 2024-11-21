#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

int main()
{
    HANDLE Ready, NotReady, Event;
    STARTUPINFO* si;
    PROCESS_INFORMATION* pi;
    int NumWord, SendNum;
    wchar_t file[50];
    wstring files;
    cout << "Enter file name: " << endl;
    wcin >> files;
    cout << "Enter number of words: " << endl;
    wcin >>NumWord;

    wstring s = L"Sender.exe " + files;
    lstrcpyW(file, s.data());

    ofstream createBinFile(files, ios::binary);
    createBinFile.close();

    Event = CreateEvent(NULL, FALSE, TRUE, L"SyncEvent");
    Ready = CreateSemaphore(NULL, 0, NumWord, L"FullSemaphore");
    NotReady = CreateSemaphore(NULL, NumWord, NumWord, L"EmptySemaphore");

    if (Event == NULL || Ready == NULL || NotReady == NULL) {
        cout << "Error." << endl;
        return GetLastError();
    }

    cout << "Enter number of senders: " << endl;
    cin >> SendNum;

    si = new STARTUPINFO[SendNum];
    pi = new PROCESS_INFORMATION[SendNum];
    HANDLE* Processors = new HANDLE[SendNum];
    ZeroMemory(si, sizeof(STARTUPINFO) * SendNum);
    for (int i = 0; i < SendNum; i++) {
        si[i].cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, file, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
            cout << "Process " << i << " error";
            return GetLastError();
        }
        Processors[i] = pi->hProcess;
    }

    if (SendNum == 1)
        WaitForSingleObject(&Processors, INFINITE);
    else
        WaitForMultipleObjects(SendNum, Processors, TRUE, INFINITE);

    char* Wordn = new char[20];
    ifstream inbin;
    ofstream outbin;
    string consolein;
    string delet;

    while (true) {
        cout << "Enter operation (read/exit):" << endl;
        cin >> consolein;

        if (consolein == "read") {
            WaitForSingleObject(Ready, INFINITE);
            WaitForSingleObject(Event, INFINITE);

            inbin.open(files, ios::binary);
            inbin.read(Wordn, 20);
            inbin >> delet;
            inbin.close();

            if (strcmp(Wordn, delet.data()) == 0)
                delet = "";

            outbin.open(files, ios::binary);
            outbin.clear();
            outbin.write(delet.data(), delet.size());
            outbin.close();

            cout << Wordn << endl;

            SetEvent(Event);
            ReleaseSemaphore(NotReady, 1, NULL);
        }
        if (consolein == "exit") {
            for (int i = 0; i < SendNum; i++) {
                TerminateProcess(pi[i].hProcess, 1);
                TerminateProcess(pi[i].hThread, 1);
            }
            break;
        }
        if (consolein != "read" && consolein != "exit") {
            cout << "Wrong command." << endl;
        }
    }

    WaitForMultipleObjects(SendNum, &pi->hProcess, TRUE, INFINITE);

    for (int i = 0; i < SendNum; i++) {
        CloseHandle(pi[i].hThread);
        CloseHandle(pi[i].hProcess);
    }

    CloseHandle(Event);
    CloseHandle(Ready);
    CloseHandle(NotReady);
    inbin.close();
    outbin.close();
    return 0;
}
