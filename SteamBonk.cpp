#include <Windows.h>
#include <Psapi.h>
#include <algorithm>
#include <filesystem>
#include <map>

bool firstBonked = false;
bool shouldExit = false;
DWORD exitDelay = 5000;
DWORD bonkTime = 0;

std::wstring targetWindowClass = L"SDL_app";
std::map<std::wstring, bool> candidateCaptions{
    {L"Steam", false},
    {L"Special Offers", false},
};

void Log(wchar_t const* msgLine) {
    OutputDebugStringW(msgLine);
}

// int main() {
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    while (!shouldExit) {
        Log(L"Enumerating\n");
        EnumWindows([](HWND wnd, LPARAM lParam) -> BOOL {
            wchar_t className[1024]{};
            DWORD procId{};
            if (GetWindowThreadProcessId(wnd, &procId)) {
                if (GetClassNameW(wnd, std::data(className), std::size(className))) {
                    if (wcsicmp(className, L"SDL_app") == 0) {
                        wchar_t exeName[65536]{};
                        if (HANDLE procHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, procId)) {
                            DWORD exeNameSize = std::size(exeName);
                            if (QueryFullProcessImageNameW(procHandle, 0, std::data(exeName), &exeNameSize)) {
                                std::filesystem::path path(exeName);
                                std::wstring stem = path.stem();
                                if (wcsicmp(stem.c_str(), L"steamwebhelper") == 0) {
                                    Log(L"Found a matching executable\n");
                                    bool isMinimzed = IsIconic(wnd);
                                    wchar_t windowTitle[65536]{};
                                    GetWindowTextW(wnd, std::data(windowTitle), std::size(windowTitle));
                                    if (auto I = candidateCaptions.find(windowTitle); I != candidateCaptions.end()) {
                                        wchar_t message[65536]{};
                                        wsprintfW(message, L"  Minimized: %d, title: \"%s\"\n", isMinimzed, windowTitle);
                                        Log(message);
                                        // MessageBoxW(nullptr, message, L"Beep boop", MB_OK);
                                        // if (!I->second) {
                                            SendMessage(wnd, WM_CLOSE, 0, 0);
                                        // }
                                        I->second = true;
                                        firstBonked = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return TRUE;
        }, 0);
        if (firstBonked) {
            if (!bonkTime) {
                bonkTime = timeGetTime();
            }
            if (timeGetTime() > bonkTime + exitDelay) {
                break;
            }
        }
        Sleep(500);
    }
    return 0;
}