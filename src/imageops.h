#ifndef IMAGE_OPS_H_
#define IMAGE_OPS_H_

#include "raylib.h"

Image create_image(unsigned char *pixels, int width, int height);
unsigned char get_pixel(Image img, int x, int y, int channel);
void set_pixel(Image img, int x, int y, int channel, unsigned char value);
Image biliear_resize(Image img, int width, int height);
void draw_line(Image img, int start_x, int start_y, int end_x, int end_y, float tickness, Color color);
void draw_line_bresenham(Image img, int start_x, int start_y, int end_x, int end_y, float tickness, Color color);

#endif // IMAGE_OPS_H_