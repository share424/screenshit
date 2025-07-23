#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include "screenshit.h"

#define HOTKEY_ID 1

int main() {
    if (!RegisterHotKey(NULL, 1, MOD_CONTROL, VK_SNAPSHOT)) {
        printf("Failed to register PrintScreen hotkey.\n");
        return 1;
    }
    printf("Listening for PrintScreen...\n");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
            printf("PrintScreen pressed!\n");
            run_screenshit();
        }
    }

    UnregisterHotKey(NULL, HOTKEY_ID);
    
    return 0;
}
#elif defined(__linux__)
    // Linux-specific code
#elif defined(__APPLE__)
    // macOS-specific code
#else
    // Other platforms (optional)
#endif