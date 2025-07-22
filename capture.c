#include "capture.h"
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void save_image_as_png(const char *filename, const Image *img) {
    int stride = img->width * 4; // 4 bytes per pixel (RGBA)
    // stbi_write_png returns 1 on success
    if (stbi_write_png(filename, img->width, img->height, 4, img->data, stride)) {
        printf("Saved PNG: %s\n", filename);
    } else {
        printf("Failed to save PNG: %s\n", filename);
    }
}

#ifdef _WIN32
#include <windows.h>

int hbitmap_to_image_rgba(HBITMAP hBitmap, HDC hDC, Image *out_img) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    int width = bmp.bmWidth;
    int height = bmp.bmHeight;

    BITMAPINFOHEADER bi;
    memset(&bi, 0, sizeof(bi));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  // Negative for top-down
    bi.biPlanes = 1;
    bi.biBitCount = 32; // Request 32-bit bitmap (BGRA)
    bi.biCompression = BI_RGB;

    size_t row_bytes = width * 4;
    size_t total_bytes = row_bytes * height;
    unsigned char *bgra = (unsigned char*)malloc(total_bytes);
    if (!bgra) return 0;

    BITMAPINFO binfo = {0};
    binfo.bmiHeader = bi;

    int res = GetDIBits(hDC, hBitmap, 0, height, bgra, &binfo, DIB_RGB_COLORS);
    if (res == 0) {
        free(bgra);
        return 0;
    }

    // Allocate output image RGBA buffer
    out_img->data = (unsigned char*)malloc(total_bytes);
    if (!out_img->data) {
        free(bgra);
        return 0;
    }

    // Convert BGRA → RGBA
    for (int i = 0; i < width * height; ++i) {
        out_img->data[i * 4 + 0] = bgra[i * 4 + 2]; // R
        out_img->data[i * 4 + 1] = bgra[i * 4 + 1]; // G
        out_img->data[i * 4 + 2] = bgra[i * 4 + 0]; // B
        out_img->data[i * 4 + 3] = bgra[i * 4 + 3]; // A
    }

    free(bgra);

    out_img->width = width;
    out_img->height = height;
    return 1;
}

Image take_screenshot() {
    HDC hScreenDC = GetDC(NULL);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    SelectObject(hMemDC, hBitmap);

    BitBlt(hMemDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY);

    Image out_img;
    hbitmap_to_image_rgba(hBitmap, hMemDC, &out_img);

    return out_img;
}
#elif defined(__linux__)
    // Linux-specific code
#elif defined(__APPLE__)
    // macOS-specific code
#else
    // Other platforms (optional)
#endif
