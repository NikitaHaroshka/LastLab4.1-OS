#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

int main(int argc, char* argv[])
{
    HANDLE Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"SyncEvent");
    HANDLE Ready = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"FullSemaphore");
    HANDLE NotReady = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"EmptySemaphore");

    if (Event == NULL || Ready == NULL || NotReady == NULL) {
        cout << "Error opening synchronization objects." << endl;
        return GetLastError();
    }

    string inputs;
    while (true) {
        cout << "Type your operation:" << endl;
        cin >> inputs;

        if (inputs == "exit") break;
        else {
            WaitForSingleObject(NotReady, INFINITE);
            WaitForSingleObject(Event, INFINITE);

            ofstream out(argv[1], ios::app, ios::binary);
            out.write(inputs.data(), 20);
            out.close();

            SetEvent(Event);
            ReleaseSemaphore(Ready, 1, NULL);
        }
    }
    return 0;
}