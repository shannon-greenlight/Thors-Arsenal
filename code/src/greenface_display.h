#include <Greenface_ui.h>
#define TERMINAL_WIDTH 76 // Display area under title box
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Greenface_ui ui(&display, PRODUCT_NAME);
