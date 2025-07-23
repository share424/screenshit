#include "capture.h"
#include "imageops.h"
#include "clipboard.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define RPNG_IMPLEMENTATION
#include "rpng.h"

#define TOOLBAR_HEIGHT 50
#define SCREENSHOT_HEIGHT 1200

bool IS_BRUSH = false;
float BRUSH_SIZE = 5.0;
Color BRUSH_COLOR = {
    .r = 255,
    .g = 0,
    .b = 0,
    .a = 255
};

bool SHOW_SAVE_MESSAGE_BOX = false;
Vector2 SAVE_MESSAGE_BOX_POS = {0};


int get_new_width(Image img, int target_height) {
    float ratio = (float)img.width / (float)img.height;
    return (int)floor(target_height * ratio);
}

int GuiIconButton(Rectangle bounds, int icon, const char *text) {
    char label[64];
    snprintf(label, sizeof(label), "#%d#%s", icon, text);
    return GuiButton(bounds, label);
}

void save_screenshit(Image img) {
    rpng_save_image("screenshit.png", img.data, img.width, img.height, 4, 8);
}

void draw_toolbar(int width, Image img)
{
    Rectangle rect_panel = {
        .x = 0,
        .y = 0,
        .width = width,
        .height = TOOLBAR_HEIGHT
    };
    GuiPanel(rect_panel, NULL);

    // TOOL
    if (GuiIconButton((Rectangle){ 10, 10, 100, 30 }, ICON_FILE_COPY, "Copy") || IsKeyPressed(KEY_C)) {
        int file_output_size = 0;
        char *file_output = rpng_save_image_to_memory(img.data, img.width, img.height, 4, 8, &file_output_size);
        copy_to_clipboard(file_output, file_output_size);
    }

    if (GuiIconButton((Rectangle){ 120, 10, 100, 30 }, ICON_FILE_SAVE_CLASSIC, "Save") || IsKeyPressed(KEY_S)) {
        save_screenshit(img);
        SAVE_MESSAGE_BOX_POS.x = GetMouseX();
        SAVE_MESSAGE_BOX_POS.y = GetMouseY();
        SHOW_SAVE_MESSAGE_BOX = true;
    }

    if (GuiIconButton((Rectangle){ 230, 10, 100, 30 }, ICON_BRUSH_CLASSIC, "Brush") || IsKeyPressed(KEY_B)) {
        IS_BRUSH = !IS_BRUSH;
    }
}

void draw_brush(Image img) {
    if (!IS_BRUSH) return;

    GuiSliderBar((Rectangle){ 420, 10, 100, 30 }, "Brush size: 0", "100", &BRUSH_SIZE, 1.0, 30.0);
    GuiColorPicker((Rectangle){ 570, 5, 100, 40 }, "Brush color", &BRUSH_COLOR);

    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();

    DrawCircle(mouse_x, mouse_y, BRUSH_SIZE, BRUSH_COLOR);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        draw_line_bresenham(img, mouse_x - delta.x, mouse_y - delta.y - TOOLBAR_HEIGHT, mouse_x, mouse_y - TOOLBAR_HEIGHT, BRUSH_SIZE, BRUSH_COLOR);
    }
}

void draw_screenshot(Texture2D texture)
{
    DrawTexture(texture, 0, TOOLBAR_HEIGHT, WHITE); 
}

Image take_screenshot() {
    CaptureImage img = take_screenshit();
    Image screenshot = create_image(img.data, img.width, img.height);
    return screenshot;
}

int run_screenshit()
{
    SetTraceLogLevel(LOG_NONE);
    Image original_image = take_screenshot();
    int width = get_new_width(original_image, SCREENSHOT_HEIGHT);
    Image img = ImageCopy(original_image);
    ImageResize(&img, width, SCREENSHOT_HEIGHT);
    
    InitWindow(img.width, img.height + TOOLBAR_HEIGHT, "ScreenShit");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Texture2D texture = LoadTextureFromImage(img);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            draw_toolbar(width, img);
            draw_screenshot(texture);
            draw_brush(img);

            if (SHOW_SAVE_MESSAGE_BOX) {
                int result = GuiMessageBox((Rectangle){ SAVE_MESSAGE_BOX_POS.x, SAVE_MESSAGE_BOX_POS.y, 250, 100 },
                    "#191#Screenshit saved!", "Screenshit saved at screenshit.png", "Nice;Cool");
                
                if (result >= 0) SHOW_SAVE_MESSAGE_BOX = false;
            }
            
        EndDrawing();

        UnloadTexture(texture);
    }

    UnloadImage(img);
    UnloadImage(original_image);
    CloseWindow();
}