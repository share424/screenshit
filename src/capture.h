#ifndef CAPTURE_H_
#define CAPTURE_H_

typedef struct {
    unsigned char *data; // RGBA
    int width;
    int height;
} CaptureImage;

CaptureImage take_screenshit();

#endif // CAPTURE_H_