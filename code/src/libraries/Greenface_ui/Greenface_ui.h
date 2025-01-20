// ensure this library description is only included once
#ifndef Greenface_ui_h
#define Greenface_ui_h

#include <Adafruit_SSD1306.h> //
#include <Adafruit_GFX.h>     //
#include <Wire.h>             // ???
#include "TerminalVT100.h"

// #include "../../../version_num.h"
// #include "hardware_defs.h"

#define LINE_0 0
#define LINE_1 20
#define LINE_2 36
#define LINE_3 52
#define STATUS_ROW "5"
#define FXN_ROW "6"
#define DEBUG_ROW "21"
#define TERMINAL_BASE 8

#define OLED_RESET 16 // Pin 15 -RESET digital signal

extern String version_num, old_status;
extern String settings_get_device_name();
extern String system_message_debug;

//
const unsigned char PROGMEM logo_bmp[] =
    {0b00000000, 0b11000000,
     0b00000001, 0b11000000,
     0b00000001, 0b11000000,
     0b00000011, 0b11100000,
     0b11110011, 0b11100000,
     0b11111110, 0b11111000,
     0b01111110, 0b11111111,
     0b00110011, 0b10011111,
     0b00011111, 0b11111100,
     0b00001101, 0b01110000,
     0b00011011, 0b10100000,
     0b00111111, 0b11100000,
     0b00111111, 0b11110000,
     0b01111100, 0b11110000,
     0b01110000, 0b01110000,
     0b00000000, 0b00110000};

//

class Greenface_ui
{
public:
  bool display_is_on = true;
  bool terminal_mirror = true;
  unsigned long inactivity_timer = 0;

  // todo use extern
  const int lines[4] = {LINE_1, LINE_2, LINE_3, LINE_0};
  TerminalVT100 t;
  Adafruit_SSD1306 *display;
  String product_name;

  Greenface_ui(Adafruit_SSD1306 *_display, String _product_name)
  {
    display = _display;
    product_name = _product_name;
  }

  void drawBitmap(int x, int y, const uint8_t *bitmap)
  {
    clearDisplay();
    // printText("Greenface",0,0,2);
    (*display).drawBitmap(x, y, bitmap, 128, 64, WHITE);
    showDisplay();
  }

#define XPOS 0 // Indexes into the 'icons' array in function below
#define YPOS 1
#define DELTAY 2

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

  int8_t f, icons[NUMFLAKES][3];
  void snowflakes(const uint8_t *bitmap)
  {

    // Initialize 'snowflake' positions
    for (f = 0; f < NUMFLAKES; f++)
    {
      icons[f][XPOS] = random(1 - LOGO_WIDTH, (*display).width());
      icons[f][YPOS] = -LOGO_HEIGHT;
      icons[f][DELTAY] = random(1, 6);
      // Serial.print(F("x: "));
      // Serial.print(icons[f][XPOS], DEC);
      // Serial.print(F(" y: "));
      // Serial.print(icons[f][YPOS], DEC);
      // Serial.print(F(" dy: "));
      // Serial.println(icons[f][DELTAY], DEC);
    }
  }

  void snowflakes_begin()
  {
    snowflakes(logo_bmp);
  }

  void begin()
  {
    t.begin();
    // SSD1306 Init
    (*display).begin(SSD1306_SWITCHCAPVCC, 0x3C); // Switch OLED
    // Clear the buffer.
    (*display).clearDisplay();
    (*display).stopscroll();
    (*display).setTextColor(WHITE, BLACK);
    (*display).ssd1306_command(SSD1306_DISPLAYOFF);
    //(*display).setRotation(2);
    //(*display).startscrollright(0,15);
    reset_inactivity_timer();
    snowflakes_begin();
  }

  void signon(const uint8_t *bitmap)
  {
    for (int i = 0; i < 48; i += 2)
    {
      drawBitmap(0, 64 - i, bitmap);
    }
    // clearDisplay();
    fill(BLACK, 16);
    // printText("Greenface Labs",0,0,1);
    printText("Greenface Labs", 0, 0, 1);
    (*display).drawBitmap(0, 16, bitmap, 128, 64, WHITE);
    showDisplay();
  }

  void clearTerminal()
  {
    t.clrScreen();
  }

  void clearDisplay()
  {
    // clr terminal here
    if (terminal_mirror)
    {
      clearTerminal();
    }
    (*display).clearDisplay();
  }

  void showDisplay()
  {
    (*display).display();
  }

  void dim(boolean dim)
  {
    (*display).dim(dim);
  }

  void display_off()
  {
    display_is_on = false;
    (*display).ssd1306_command(SSD1306_DISPLAYOFF);
  }

  void display_on()
  {
    display_is_on = true;
    (*display).ssd1306_command(SSD1306_DISPLAYON);
  }

  void reset_inactivity_timer()
  {
    inactivity_timer = millis();
  }

  void _snowflake_frame()
  {
    (*display).clearDisplay(); // Clear the buffer

    // Draw each snowflake:
    for (f = 0; f < NUMFLAKES; f++)
    {
      (*display).drawBitmap(icons[f][XPOS], icons[f][YPOS], logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
    }

    (*display).display(); // Show the buffer on the screen
    // delay(200);           // Pause for 1/10 second

    // Then update coordinates of each flake...
    for (f = 0; f < NUMFLAKES; f++)
    {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= (*display).height())
      {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS] = random(1 - LOGO_WIDTH, (*display).width());
        icons[f][YPOS] = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }

  int snowflake_count = 0;
  void snowflake_frame()
  {
    if (snowflake_count++ > 10)
    {
      _snowflake_frame();
      snowflake_count = 0;
    }
  }

  bool screen_saved = false;
  void check_inactivity_timer(int timeout)
  {
    bool timed_out = millis() > (inactivity_timer + timeout * 60000);
    bool way_timed_out = millis() > (inactivity_timer + ((timeout + 15) * 60000));
    if (timed_out) // Screen Saver
    {
      if (!screen_saved)
      {
        screen_saver();
      }
    }
    if (way_timed_out)
    {
      display_off();
    }
    else
    {
      if (screen_saved)
      {
        snowflake_frame();
      }
    }
  }

  void screen_saver()
  {
    fill(BLACK, 0);
    showDisplay();
    printLine(F("Screen Saver"), 0, 1);
    delay(1000);
    screen_saved = true;
    // display_off();
    dim(true);
    // do_snowflakes(true);
  }

  void all_off()
  {
    display_off();
    terminal_mirror = false;
  }

  void all_on(bool mirror = true)
  {
    display_on();
    dim(false);
    terminal_mirror = mirror;
    screen_saved = false;
  }

  void terminal_clrDown(String row)
  {
    if (terminal_mirror)
    {
      t.clrDown(row);
    }
  }

  void terminal_clrToEOL()
  {
    if (terminal_mirror)
    {
      t.clrToEOL();
    }
  }

  void terminal_setRow(String row)
  {
    if (terminal_mirror)
    {
      t.setRow(row);
    }
  }

  void terminal_setCursor(String row, String char_num)
  {
    if (terminal_mirror)
    {
      t.setCursor(row, char_num);
    }
  }

  void terminal_print(String s)
  {
    if (terminal_mirror)
    {
      Serial.print(s);
    }
  }

  void terminal_println(String s)
  {
    if (terminal_mirror)
    {
      Serial.println(s);
    }
  }

  void terminal_print(char c)
  {
    if (terminal_mirror)
    {
      Serial.print(c);
    }
  }

  void terminal_printRow(String text, String row)
  {
    if (terminal_mirror && row != "-1")
    {
      t.setRow(row);
      t.clrToEOL();
      t.println(text);
    }
  }

#define DEBUG_APPEND true
  void terminal_debug(String text, bool append = false)
  {
    system_message_debug = text; // for USB
    static int row = 0;
    if (append)
    {
      String new_row = String(21 + row);
      terminal_printRow(text + new_row, new_row);
    }
    else
    {
      row = 0;
      terminal_clrDown(DEBUG_ROW);
      terminal_printRow(text, DEBUG_ROW);
    }
    row++;
  }

  void terminal_printText(String text, int line)
  {
    String row = String(TERMINAL_BASE + ((line - 20) / 8));
    terminal_printRow(text, row);
  }

  void printText(String s, int x = 0, int y = 0, int fontsize = 1)
  {
    if (display_is_on)
    {
      (*display).setCursor(x, y);
      (*display).setTextSize(fontsize);
      (*display).println(s);
      (*display).display();
    }
    if (terminal_mirror)
    {
      terminal_printText(s, y);
    }
  }

  void printScreen(String head, String body, String foot = "")
  {
    printText(head, 0, 0, 2);
    printText(body, 0, 20, 3);
    printText(foot, 0, 48, 2);
  }

  void printLine(String text, int line, int fontsize = 2)
  {
    printText(text, 0, line, fontsize);
    // terminalPrintParam(row, label, String(buf));
  }

  int constrainX(int x)
  {
    return constrain(x, 0, 127);
  }

  int constrainY(int y)
  {
    return constrain(y, LINE_1, 63);
  }

  void drawPixel(int x, int y, int color = WHITE, bool constrain_vals = false)
  {
    if (display_is_on)
    {
      if (constrain_vals)
      {
        x = constrainX(x);
        y = constrainX(y);
      }
      (*display).drawPixel(x, y, color);
    }
  }

  void drawHLine(int y, int color = WHITE)
  {
    if (display_is_on)
    {
      (*display).drawFastHLine(0, y, 128, color);
    }
  }

  void drawVLine(int x, int color = WHITE)
  {
    if (display_is_on)
    {
      (*display).drawFastVLine(x, LINE_1, 64, color);
    }
  }

  void drawVHash(int x, int y, int color = WHITE)
  {
    if (display_is_on)
    {
      (*display).drawFastVLine(x, constrainY(y - 4), 11, color);
    }
  }

  void drawXHash(int x, int y, int color = WHITE)
  {
    if (display_is_on)
    {
      // upper left
      drawPixel(x - 2, y - 2, WHITE, true);
      drawPixel(x - 1, y - 1, WHITE, true);

      // upper right
      drawPixel(x + 2, y - 2, WHITE, true);
      drawPixel(x + 1, y - 1, WHITE, true);

      // lower left
      drawPixel(x - 2, y + 2, WHITE, true);
      drawPixel(x - 1, y + 1, WHITE, true);

      // lower right
      drawPixel(x + 2, y + 2, WHITE, true);
      drawPixel(x + 1, y + 1, WHITE, true);
    }
  }

  void hiliteLine(uint16_t line_num, int num_lines = 3, int offset = 9)
  {
    String s = "---------------------";
    String blank = "                     ";
    for (int i = 0; i < num_lines; i++)
    {
      drawHLine(lines[i] + offset, BLACK);
      if (terminal_mirror)
      {
        terminal_printText(blank, lines[i] + offset);
      }
    }
    // Serial.println("Hilighting: "+String(lines[line_num]+offset));
    drawHLine(lines[line_num] + offset, WHITE);
    if (terminal_mirror)
    {
      terminal_printText(s, lines[line_num] + offset);
    }
  }

  void hiliteChar(uint16_t line_num, uint8_t char_num)
  {
    int num_lines = 3;
    int offset = 9;
    for (int i = 0; i < num_lines; i++)
    {
      drawHLine(lines[i] + offset, BLACK);
    }
    // Serial.println("Hilighting: " + String(lines[line_num] + offset));
    printText("_", char_num, lines[line_num] + offset, 1);
    // drawHLine(lines[line_num]+offset,WHITE);
  }

  void fill(int color = WHITE, int offset = 0)
  {
    for (int i = offset; i < 64; i++)
    {
      (*display).drawFastHLine(0, i, 128, color);
    }
    // t.clrDown(String((offset / 16) + 6));
    //  terminal_debug("fill offset: " + String(offset));
  }

  void graphData(int *data)
  {
    for (int i = 0; i < 128; i++)
    {
      drawPixel(i, data[i], WHITE);
    }
  }

  int fit_data(int intval, int scale)
  {
    float val = float(intval) / scale;
    return max(16, 63 - (int(48 * val)));
  }

  void graph_waveform(uint16_t *waveform_data, byte data_len, int fs)
  {
    // ui.terminal_debug("Graph waveform: " + String(output_num));
    int data[SSD1306_LCDWIDTH];

    int index = 0; // index into source waveform data
    float index_inc = SSD1306_LCDWIDTH / float(data_len);

    fill(BLACK, 16);

    for (int i = 0; i < SSD1306_LCDWIDTH; i++)
    {
      data[i] = fit_data(waveform_data[index], fs);
      if (i + 1 > (index + 1) * index_inc)
      {
        index++;
      }
    }
    graphData(data);
    showDisplay();
  }

  const int basex = 116;
  const int basey = 14;

  void printWiFiBar(int x, int h)
  {
    if (display_is_on)
    {
      (*display).drawFastVLine(x, basey - h, h, WHITE);
      (*display).drawFastVLine(x + 1, basey - h, h, WHITE);
    }
  }

  void printWiFiBars(long rssi)
  {
    if (rssi > -80)
      printWiFiBar(basex, 3);
    if (rssi > -70)
      printWiFiBar(basex + 3, 6);
    if (rssi > -60)
      printWiFiBar(basex + 6, 9);
    if (rssi > -50)
      printWiFiBar(basex + 9, 12);
    showDisplay();
  }

  void test()
  {
    fill();
    showDisplay();
  }

  void terminalPrintParam(String row, String label, String val)
  {
    t.printVal(row, label, val);
  }

  void terminalSplash()
  {
    // Use Putty to communicate (VT100 terminal)
    t.clrScreen();
    t.setCursor("1", "1");
    const int stars = 76;

    t.printChars(stars, "*");
    t.println("");
    t.printTitle(stars, product_name + "  by  GREENFACE LABS ");
    t.setCursor("2", String(stars));
    t.println("*");
    t.printTitle(stars, version_num + " " + settings_get_device_name());
    t.setCursor("3", String(stars));
    t.println("*");
    t.printChars(stars, "*");
    t.println("");
  }

  void newFxn(String fxn)
  {
    clearDisplay();
    if (terminal_mirror)
    {
      terminalSplash();
      t.clrDown(FXN_ROW);
    }
    old_status = ""; // force reprint of status_string
    printLine(fxn, 0, fxn.length() > 9 ? 1 : 2);
  }

  String format_float(float val, int digits, const char *format = "%.2f")
  {
    char buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // buffer to print up to 9 digits
    snprintf(buf, digits + 1, format, val);
    return String(buf);
  }

  void printParam(String label, int param, int digits, const char *format, uint16_t line_num, int font_size = 1, String suffix = "")
  {
    char buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // buffer to print up to 9 digits
    snprintf(buf, digits + 1, format, param);
    printLine(label + String(buf) + suffix, lines[line_num], font_size);
  }

  void underline_char(byte char_num, byte line_num, int font_size = 2, int offset = 0, const char *c = "-")
  {
    String prefix = "";
    for (int i = 0; i < char_num; i++)
    {
      prefix += " ";
    }
    String sval = prefix + c;
    // Serial.println("Prefix: " + prefix);
    // drawHLine(lines[line_num] + offset, BLACK);
    // fill(BLACK, lines[line_num] + offset);
    int num_blanks = 21 - sval.length();
    for (int i = 0; i < num_blanks; i++)
    {
      sval += " ";
    }
    printLine(sval, lines[line_num] + offset, font_size);
    //(*display).println(prefix + "-");
  }

  uint8_t center(String s, int font_size = 1)
  {
    return (128 - s.length() * 6 * font_size) / 2;
  }

  void printCentered(String s, uint8_t y, int font_size = 1)
  {
    printText(s, center(s, font_size), y, font_size);
  }

  void splash()
  {
    String temp;
    clearDisplay();
    printCentered(product_name, 0);
    for (int i = 0; i < product_name.length(); i++)
    {
      temp += "_";
    }
    printCentered(temp, 8);
    printCentered(version_num, 16);
    temp = "Hi, my name is";
    printCentered(temp, 32);
    temp = settings_get_device_name();
    printCentered(temp, 40);

    // printText(product_name, center(product_name), 0, 1);
    // for (int i = 0; i < product_name.length(); i++)
    // {
    //   temp += "_";
    // }
    // printText(temp, center(temp), 8, 1);
    // printText(version_num, center(version_num), 16, 1);
    // temp = "Hi, my name is";
    // printText(temp, center(temp), 32, 1);
    // temp = settings_get_device_name();
    // printText(temp, center(temp), 40, 1);

    terminalSplash();
  }
};

#endif
