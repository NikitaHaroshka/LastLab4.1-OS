#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

int main(int argc, char* argv[])
{
    HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"SyncEvent");
    HANDLE hFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"FullSemaphore");
    HANDLE hEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"EmptySemaphore");

    if (hEvent == NULL || hFull == NULL || hEmpty == NULL) {
        cout << "Error opening synchronization objects." << endl;
        return GetLastError();
    }

    string ans;
    while (true) {
        cout << "Type your operation:" << endl;
        cin >> ans;

        if (ans == "exit") break;
        else {
            WaitForSingleObject(hEmpty, INFINITE);
            WaitForSingleObject(hEvent, INFINITE); // Ожидание события

            ofstream out(argv[1], ios::app, ios::binary);
            out.write(ans.data(), 20);
            out.close();

            SetEvent(hEvent); // Устанавливаем событие
            ReleaseSemaphore(hFull, 1, NULL);
        }
    }
    return 0;
}
