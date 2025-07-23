#include "clipboard.h"

#ifdef _WIN32
#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void create_png_clipboard(const unsigned char *png_data, size_t png_size);

void copy_to_clipboard(char *file_output, int file_output_size) {
    if (OpenClipboard(NULL)) {
        EmptyClipboard();

        // Provide CF_DIB for legacy applications like Paint.
        // create_dib_clipboard(pixels, width, height);

        // Provide PNG format for modern applications if it was generated successfully.
        create_png_clipboard(file_output, file_output_size);

        CloseClipboard();
    }
}

void create_png_clipboard(const unsigned char *png_data, size_t png_size) {
    // Register the "PNG" clipboard format. This is a common convention.
    UINT cf_png = RegisterClipboardFormatA("PNG");
    if (cf_png == 0) {
        fprintf(stderr, "Failed to register PNG clipboard format.\n");
        return;
    }

    // Allocate global memory for the PNG data.
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, png_size);
    if (!hMem) {
        return;
    }

    // Lock the memory and copy the PNG data into it.
    void* pMem = GlobalLock(hMem);
    if (!pMem) {
        GlobalFree(hMem);
        return;
    }
    memcpy(pMem, png_data, png_size);
    GlobalUnlock(hMem);

    // Set the clipboard data. If successful, the system now owns the memory.
    if (!SetClipboardData(cf_png, hMem)) {
        // If it fails, we are responsible for freeing the memory we allocated.
        GlobalFree(hMem);
    }
}

#elif defined(__linux__)
    // Linux-specific code
#elif defined(__APPLE__)
    // macOS-specific code
#else
    // Other platforms (optional)
#endif
