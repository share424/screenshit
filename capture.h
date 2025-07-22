#ifndef CAPTURE_H_
#define CAPTURE_H_

typedef struct {
    unsigned char *data; // RGBA
    int width;
    int height;
} Image;

Image take_screenshot();

void save_image_as_png(const char *filename, const Image *img);

#endif // CAPTURE_H_