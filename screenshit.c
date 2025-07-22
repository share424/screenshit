#include <stdio.h>
#include <stdlib.h>
#include "capture.h"


int main() {
    Image img = take_screenshot();
    save_image_as_png("screenshot.png", &img);

    free(img.data);
    return 0;
}