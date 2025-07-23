#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "imageops.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

Image create_image(unsigned char *pixels, int width, int height) {
    Image im = {
        .data = pixels,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    return im;
}

void clamp(Image im, int *x, int *y, int *c)
{
    *x = MIN(MAX(*x, 0), im.width - 1);
    *y = MIN(MAX(*y, 0), im.height - 1);
    *c = MIN(MAX(*c, 0), 4 - 1);
}

unsigned char get_pixel(Image img, int x, int y, int channel) {
    clamp(img, &x, &y, &channel);
    unsigned char *pixels = (unsigned char *)img.data;
    return pixels[(y * img.width * 4) + (x * 4) + channel];
}

void set_pixel(Image img, int x, int y, int channel, unsigned char value) {
    clamp(img, &x, &y, &channel);
    unsigned char *pixels = (unsigned char *)img.data;
    pixels[(y * img.width * 4) + (x * 4) + channel] = value;
}

float bilinear_interpolate(Image im, float x, float y, int c)
{
    float lower_x = floor(x);
    float upper_x = ceil(x);
    float lower_y = floor(y);
    float upper_y = ceil(y);
    
    float v1 = get_pixel(im, (int)lower_x, (int)lower_y, c);
    float v2 = get_pixel(im, (int)upper_x, (int)lower_y, c);
    float v3 = get_pixel(im, (int)lower_x, (int)upper_y, c);
    float v4 = get_pixel(im, (int)upper_x, (int)upper_y, c);

    float a1 = (upper_x - x) * (upper_y - y);
    float a2 = (x - lower_x) * (upper_y - y);
    float a3 = (upper_x - x) * (y - lower_y);
    float a4 = (x - lower_x) * (y - lower_y);

    return v1*a1 + v2*a2 + v3*a3 + v4*a4;
}

Image biliear_resize(Image img, int width, int height) {
    size_t row_bytes = width * 4;
    size_t total_bytes = row_bytes * height;
    Image resized_image = {
        .data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4),
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    float x_step = (float)img.width / (float)width;
    float y_step = (float)img.height / (float)height;

    float x_offset = (x_step/2) - 0.5;
    float y_offset = (y_step/2) - 0.5;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int c = 0; c<4; ++c) {
                unsigned char v = bilinear_interpolate(img, x*x_step + x_offset, y*y_step + y_offset, c);
                set_pixel(resized_image, x, y, c, v);
            }
        }
    }

    return resized_image;
}

float distance(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    return sqrtf(powf(dx, 2) + powf(dy, 2));
}

void draw_point(Image img, int x, int y, float tickness, Color color) {
    for (int i = y - tickness; i <= y + tickness; ++i) {
        for (int j = x - tickness; j <= x + tickness; ++j) {
            if (distance(x, y, j, i) > tickness) continue;
            set_pixel(img, j, i, 0, color.r);
            set_pixel(img, j, i, 1, color.g);
            set_pixel(img, j, i, 2, color.b);
            set_pixel(img, j, i, 3, color.a);
        }
    }

}

void draw_line(Image img, int start_x, int start_y, int end_x, int end_y, float tickness, Color color) {
    printf("(%d, %d) -> (%d, %d)\n", start_x, start_y, end_x, end_y);
    // swap if end_x < start_y
    if (end_x < start_x) {
        int temp = start_x;
        start_x = end_x;
        end_x = temp;

        temp = start_y;
        start_y = end_y;
        end_y = temp;
    }

    // vertical lines
    if (end_x == start_x) {
        if (end_y < start_y) {
            int temp = start_y;
            start_y = end_y;
            end_y = temp;
        }
        for (int y = start_y; y <= end_y; ++y) {
            draw_point(img, start_x, y, tickness, color);
        }

        return;
    }


    float slope = (float)(end_y - start_y) / (float)(end_x - start_x);
    for (int x = start_x; x <= end_x; ++x) {
        float y = (slope * (float)(x - start_x)) + (float)start_y;
        draw_point(img, x, (int)y, tickness, color);
    }
}

void plot_line_low(Image img, int start_x, int start_y, int end_x, int end_y, float tickness, Color color) {
    int dx = end_x - start_x;
    int dy = end_y - start_y;

    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int D = (2 * dy) - dx;
    int y = start_y;

    for (int x = start_x; x <= end_x; ++x) {
        draw_point(img, x, y, tickness, color);
        if (D > 0) {
            y = y + yi;
            D = D + (2 * (dy - dx));
        } else {
            D = D + 2*dy;
        }
    }
}

void plot_line_high(Image img, int start_x, int start_y, int end_x, int end_y, float tickness, Color color) {
    int dx = end_x - start_x;
    int dy = end_y - start_y;

    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int D = (2 * dx) - dy;
    int x = start_x;

    for (int y = start_y; y <= end_y; ++y) {
        draw_point(img, x, y, tickness, color);
        if (D > 0) {
            x = x + xi;
            D = D + (2 * (dx - dy));
        } else {
            D = D + 2*dx;
        }
    }
}

void draw_line_bresenham(Image img, int start_x, int start_y, int end_x, int end_y, float tickness, Color color) {
    if (abs(end_y - start_y) < abs(end_x - start_x)) {
        if (start_x > end_x) {
            plot_line_low(img, end_x, end_y, start_x, start_y, tickness, color);
        } else {
            plot_line_low(img, start_x, start_y, end_x, end_y, tickness, color);
        }
    } else {
        if (start_y > end_y) {
            plot_line_high(img, end_x, end_y, start_x, start_y, tickness, color);
        } else {
            plot_line_high(img, start_x, start_y, end_x, end_y, tickness, color);
        }
    }
}