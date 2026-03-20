#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> 

// Static variables to store the original window procedure and the pause state
static WNDPROC OriginalWndProc = NULL;
static bool* externalPauseFlag = nullptr;

/**
 * Custom Window Procedure (Hook)
 * This function intercepts Windows messages before they reach the game engine.
 */
LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        // Triggered when the user starts moving or resizing the window
    case WM_ENTERSIZEMOVE:
        if (externalPauseFlag) *externalPauseFlag = true;
        break;

        // Triggered when the user stops moving or resizing the window
    case WM_EXITSIZEMOVE:
        if (externalPauseFlag) *externalPauseFlag = false;
        break;
    }

    // Always call the original Window Procedure to ensure the OS continues normal operation
    return CallWindowProc(OriginalWndProc, hwnd, uMsg, wParam, lParam);
}

/**
 * AttachHook: Connects the custom logic to the existing window.
 * @param windowHandle: A pointer to the window (HWND).
 * @param pauseFlag: A pointer to a boolean used to control the game's pause state.
 */
extern "C" void AttachHook(void* windowHandle, bool* pauseFlag) {
    if (windowHandle == nullptr) return;

    externalPauseFlag = pauseFlag;
    HWND hwnd = (HWND)windowHandle;

    // Subclassing: Replace the original window procedure with our custom 'MyWindowProc'
    // and store the pointer to the original function to keep the window functional.
    OriginalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MyWindowProc);
}
#endif