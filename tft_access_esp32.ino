// This demo should work with most Adafruit TFT libraries
// If you are not using a shield,  use a full Adafruit constructor()
// e.g. Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//david
#define LCD_RD  2  //LED
#define LCD_WR  4
#define LCD_RS 15  //hard-wired to A2 (GPIO35) 
#define LCD_CS 33  //hard-wired to A3 (GPIO34)
#define LCD_RST 32 //hard-wired to A4 (GPIO36)

#include <SPI.h>          // f.k. for Arduino-1.5.2
//lib for drawing all elements
#include "Adafruit_GFX.h"// Hardware-specific library
//lib override to match pinouts
#include <MCUFRIEND_kbv.h>




MCUFRIEND_kbv tft(LCD_CS, LCD_RS, LCD_WR, LCD_RD, LCD_RST);

//oled display (not used much, just demonstration)
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define gfxWidth 128
#define gfxHeight 64
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_address 0x3C
Adafruit_SSD1306 display(gfxWidth, gfxHeight, &Wire, OLED_RESET);

//Touchscreen
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 10000

//from calibration run
const int XP=27,XM=15,YP=4,YM=14; //240x320 ID=0x7575
//const int TS_LEFT=927,TS_RT=151,TS_TOP=963,TS_BOT=146;
//my manual calibration
const int TS_LEFT=595,TS_RT=-2557,TS_TOP=763,TS_BOT=-2552;

TouchScreen ts(XP, YP, XM, YM, 300);   //re-initialised after diagnose
TSPoint tp;                            //global point
#define PORTRAIT  0
#define LANDSCAPE 1
#define TOUCH_ORIENTATION  PORTRAIT

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

void setup(void);
void loop(void);
unsigned long testFillScreen();
unsigned long testText();
unsigned long testLines(uint16_t color);
unsigned long testFastLines(uint16_t color1, uint16_t color2);
unsigned long testRects(uint16_t color);
unsigned long testFilledRects(uint16_t color1, uint16_t color2);
unsigned long testFilledCircles(uint8_t radius, uint16_t color);
unsigned long testCircles(uint8_t radius, uint16_t color);
unsigned long testTriangles();
unsigned long testFilledTriangles();
unsigned long testRoundRects();
unsigned long testFilledRoundRects();
void progmemPrint(const char *str);
void progmemPrintln(const char *str);

void runtests(void);

uint16_t g_identifier;

Adafruit_GFX_Button on_btn, off_btn;
int pixel_x, pixel_y;     //Touch_getXY() updates global vars
//digital pad buttons
Adafruit_GFX_Button buttons[10];

int dispx;
//void readResistiveTouch(void)
//{
//    tp = ts.getPoint();
//    pinMode(YP, OUTPUT);      //restore shared pins
//    pinMode(XM, OUTPUT);
//    //digitalWrite(YP, HIGH);  //because TFT control pins
//    //digitalWrite(XM, HIGH);
//    //    Serial.println("tp.x=" + String(tp.x) + ", tp.y=" + String(tp.y) + ", tp.z =" + String(tp.z));
//}
void draw_digit_pad( int x_origin , int y_origin )
{
    //x y w h
    int row_height = 60;
    int button_height = 50;
    buttons[0].initButton(&tft,  x_origin + 40 + 80*1, y_origin + 30 + row_height * 3, 80, button_height, CYAN, BLACK, WHITE, "0", 2);
    buttons[1].initButton(&tft,  x_origin + 40, y_origin + 30, 80, button_height, CYAN, BLACK, WHITE, "1", 2);
    buttons[2].initButton(&tft,  x_origin + 80*1 + 40, y_origin + 30, 80, button_height, CYAN, BLACK, WHITE, "2", 2);
    buttons[3].initButton(&tft,  x_origin + 80*2 + 40, y_origin + 30, 80, button_height, CYAN, BLACK, WHITE, "3", 2);
    buttons[4].initButton(&tft,  x_origin + 40, y_origin + 30 + row_height * 1, 80, button_height, CYAN, BLACK, WHITE, "4", 2);
    buttons[5].initButton(&tft,  x_origin + 80*1 + 40, y_origin + 30 + row_height * 1, 80, button_height, CYAN, BLACK, WHITE, "5", 2);
    buttons[6].initButton(&tft,  x_origin + 80*2 + 40, y_origin + 30 + row_height * 1, 80, button_height, CYAN, BLACK, WHITE, "6", 2);
    buttons[7].initButton(&tft,  x_origin + 40, y_origin + 30 + row_height * 2, 80, button_height, CYAN, BLACK, WHITE, "7", 2);
    buttons[8].initButton(&tft,  x_origin + 80*1 + 40, y_origin + 30 + row_height * 2, 80, button_height, CYAN, BLACK, WHITE, "8", 2);
    buttons[9].initButton(&tft,  x_origin + 80*2 + 40, y_origin + 30 + row_height * 2, 80, button_height, CYAN, BLACK, WHITE, "9", 2);
    
    buttons[0].drawButton(false);
    buttons[1].drawButton(false);
    buttons[2].drawButton(false);
    buttons[3].drawButton(false);
    buttons[4].drawButton(false);
    buttons[5].drawButton(false);
    buttons[6].drawButton(false);
    buttons[7].drawButton(false);  
    buttons[8].drawButton(false);
    buttons[9].drawButton(false);   

}

void check_buttons( int x_check, int y_check  )
{
  
  for( int i = 0; i<10; i++ )
  {
    buttons[i].press( buttons[i].contains(x_check, y_check));
    Serial.println( "press " + String(i) + ": " + String( buttons[i].contains(x_check, y_check) ) );
    if( buttons[i].contains( x_check, y_check ) )
    {
      if( buttons[i].justPressed() )
      {
        buttons[i].drawButton( true );
        on_digit( i );
      }
    }else{
      if( buttons[i].justReleased() )
      {
        buttons[i].drawButton( false );
      }
    }
  }
}
//input 
String input_chars = "";
String password = "12345";
//input display
#define INPUT_CURSOR_X 10
#define INPUT_CURSOR_Y 35
#define INPUT_SIZE 3
#define INPUT_COLOR GREEN
#define INPUT_STEP 10

#define INPUT_WIDTH 220
#define INPUT_HEIGHT 50
#define INPUT_BORDER_COLOR GREEN
#define INPUT_BACK_COLOR BLACK

int cursor_current_x = INPUT_CURSOR_X;
void open_lock(void)
{
}
void restart_input_text(void)
{
  input_chars = "";
  cursor_current_x = INPUT_CURSOR_X;
  tft.fillRect(INPUT_CURSOR_X, INPUT_CURSOR_Y, INPUT_WIDTH - 10 , 30, INPUT_BACK_COLOR);
}

void append_input( String Char )
{//append password and check it
  
  input_chars += Char;
 
}

void input_prepare(void)
{
    //input text
    tft.setCursor(cursor_current_x, INPUT_CURSOR_Y);
    tft.setTextColor(INPUT_COLOR);
    //tft.setTextColor(uint16_t color, uint16_t backgroundcolor);
    tft.setTextSize(INPUT_SIZE);
    tft.setTextWrap(false);
        
}
void input_space_prepare(void)
{
  //prepares input rectangle
  tft.drawRoundRect( 5, 30 , INPUT_WIDTH, INPUT_HEIGHT, 5, INPUT_BORDER_COLOR );
  //input text
  tft.setCursor(5, 5);
  tft.setTextColor(CYAN);
  //tft.setTextColor(uint16_t color, uint16_t backgroundcolor);
  tft.setTextSize(2);
  tft.setTextWrap(false);
  tft.print("Enter Access Code:");
}

void on_digit(int digit)
{
  input_prepare();
  tft.print( String( digit ) );
  cursor_current_x = tft.getCursorX();
  //append password
  input_chars += String( digit );
  //check if correct
  if( input_chars.length() == password.length() )
  {
      //check if correct
      if( input_chars.equals(password) )
      {
          //open
          open_lock();
          //clear
          restart_input_text();
      }else
      {
          //clear
          //clear rectangle
          //clear cursor
          //clear input
          restart_input_text();
      }
  }
}

void centerprint(const char *s, int y)
{
    int len = strlen(s) * 0;
    tft.setTextColor(WHITE, RED);
    tft.setCursor((dispx - len) / 2, y);
    tft.print(s);
}
void centertitle(const char *s)
{
    tft.fillScreen(BLACK);
    tft.fillRect(0, 0, dispx, 14, RED);
    tft.fillRect(0, 14, dispx, 1, WHITE);
    centerprint(s, 1);
    tft.setCursor(0, 30);
    tft.setTextColor(WHITE, BLACK);
}
bool Touch_getXY(void)
{
    
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    //digitalWrite(YP, HIGH);   //because TFT control pins
    //digitalWrite(XM, HIGH);
    //Serial.println( "pressure: " + String(p.z) );
    bool pressed = (p.z > 0 || p.z < 0);//-p.z > MINPRESSURE && -p.z < MAXPRESSURE);
    if (pressed) {
        Serial.println( "raw point: x: " + String(p.x) + " y: " + String(p.y) );
        String s = "x:" + String(p.x) + " y:" + String(p.y);
        char charBuf[s.length()+1];
        // Copy it over 
        s.toCharArray(charBuf, s.length()+1 );
       
        //centertitle( charBuf );
        
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

extern const uint8_t hanzi[];
void showhanzi(unsigned int x, unsigned int y, unsigned char index)
{
    uint8_t i, j, c, first = 1;
    uint8_t *temp = (uint8_t*)hanzi;
    uint16_t color;
    tft.setAddrWindow(x, y, x + 31, y + 31); //设置区域
    temp += index * 128;
    for (j = 0; j < 128; j++)
    {
        c = pgm_read_byte(temp);
        for (i = 0; i < 8; i++)
        {
            if ((c & (1 << i)) != 0)
            {
                color = RED;
            }
            else
            {
                color = BLACK;
            }
            tft.pushColors(&color, 1, first);
            first = 0;
        }
        temp++;
    }
}

void setup(void) {
    Serial.begin(115200);

    //OLED display
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_address)) { 
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    Wire.begin();
    display.display();
    delay(500);
//    // Clear the buffer
//    display.clearDisplay();
//    display.setTextSize(3);
//    display.setCursor(0, 0);
//    display.println("Test");
//    delay(100);
//    display.display();
//    delay(100);
    
    uint32_t when = millis();
    //    while (!Serial) ;   //hangs a Leonardo until you connect a Serial
    if (!Serial) delay(5000);           //allow some time for Leonardo
    Serial.println("Serial took " + String((millis() - when)) + "ms to start");
    //    tft.reset();                 //hardware reset
    uint16_t ID = tft.readID(); //
    Serial.print("ID = 0x");
    Serial.println(ID, HEX);
    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
//    ID = 0x9329;                             // force ID
    tft.begin(ID);
    dispx = tft.width();
    tft.fillScreen(BLACK);
    tft.setRotation(TOUCH_ORIENTATION);
    //on_btn.initButton(&tft,  60, 200, 100, 40, WHITE, CYAN, BLACK, "ON", 2);
    //off_btn.initButton(&tft, 180, 200, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
    draw_digit_pad(0,85);
    input_space_prepare();
    input_prepare();
    //on_btn.drawButton(false);
    //off_btn.drawButton(false);
    Serial.println("setup completed");
    Serial.println("LCD_D0 pin " + String( CHECK_MCU ) );
}

#if defined(MCUFRIEND_KBV_H_)
uint16_t scrollbuf[320];    // my biggest screen is 320x480
#define READGRAM(x, y, buf, w, h)  tft.readGRAM(x, y, buf, w, h)
#else
uint16_t scrollbuf[320];    // Adafruit only does 240x320
// Adafruit can read a block by one pixel at a time
int16_t  READGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)
{
    uint16_t *p;
    for (int row = 0; row < h; row++) {
        p = block + row * w;
        for (int col = 0; col < w; col++) {
            *p++ = tft.readPixel(x + col, y + row);
        }
    }
}
#endif

void windowScroll(int16_t x, int16_t y, int16_t wid, int16_t ht, int16_t dx, int16_t dy, uint16_t *buf)
{
    if (dx) for (int16_t row = 0; row < ht; row++) {
            READGRAM(x, y + row, buf, wid, 1);
            tft.setAddrWindow(x, y + row, x + wid - 1, y + row);
            tft.pushColors(buf + dx, wid - dx, 1);
            tft.pushColors(buf + 0, dx, 0);
        }
    if (dy) for (int16_t col = 0; col < wid; col++) {
            READGRAM(x + col, y, buf, 1, ht);
            tft.setAddrWindow(x + col, y, x + col, y + ht - 1);
            tft.pushColors(buf + dy, ht - dy, 1);
            tft.pushColors(buf + 0, dy, 0);
        }
}

void printmsg(int row, const char *msg)
{
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(0, row);
    tft.println(msg);
}

void loop(void) {

    bool down = Touch_getXY();
    //Serial.println( "touche: " + String(down) );
    if( down ){
      Serial.println("x: " + String(pixel_x) + "y: " + String(pixel_y) );
      //tft.fillRect(pixel_x, pixel_y, 10, 10, GREEN);
      check_buttons( pixel_x, pixel_y  );
    }

//    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
//    off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
//    if (on_btn.justReleased())
//        on_btn.drawButton();
//    if (off_btn.justReleased())
//        off_btn.drawButton();
//    if (on_btn.justPressed()) {
//        on_btn.drawButton(true);
//        tft.fillRect(40, 80, 160, 80, GREEN);
//    }
//    if (off_btn.justPressed()) {
//        off_btn.drawButton(true);
//        tft.fillRect(40, 80, 160, 80, RED);
//    }
  
}

typedef struct {
    PGM_P msg;
    uint32_t ms;
} TEST;
TEST result[12];

#define RUNTEST(n, str, test) { result[n].msg = PSTR(str); result[n].ms = test; delay(500); }

void runtests(void)
{
    uint8_t i, len = 24, cnt;
    uint32_t total;
    RUNTEST(0, "FillScreen               ", testFillScreen());
    RUNTEST(1, "Text                     ", testText());
    RUNTEST(2, "Lines                    ", testLines(CYAN));
    RUNTEST(3, "Horiz/Vert Lines         ", testFastLines(RED, BLUE));
    RUNTEST(4, "Rectangles (outline)     ", testRects(GREEN));
    RUNTEST(5, "Rectangles (filled)      ", testFilledRects(YELLOW, MAGENTA));
    RUNTEST(6, "Circles (filled)         ", testFilledCircles(10, MAGENTA));
    RUNTEST(7, "Circles (outline)        ", testCircles(10, WHITE));
    RUNTEST(8, "Triangles (outline)      ", testTriangles());
    RUNTEST(9, "Triangles (filled)       ", testFilledTriangles());
    RUNTEST(10, "Rounded rects (outline)  ", testRoundRects());
    RUNTEST(11, "Rounded rects (filled)   ", testFilledRoundRects());

    tft.fillScreen(BLACK);
    tft.setTextColor(GREEN);
    tft.setCursor(0, 0);
    uint16_t wid = tft.width();
    if (wid > 176) {
        tft.setTextSize(2);
#if defined(MCUFRIEND_KBV_H_)
        tft.print("MCUFRIEND ");
#if MCUFRIEND_KBV_H_ != 0
        tft.print(0.01 * MCUFRIEND_KBV_H_, 2);
#else
        tft.print("for");
#endif
        tft.println(" UNO");
#else
        tft.println("Adafruit-Style Tests");
#endif
    } else len = wid / 6 - 8;
    tft.setTextSize(1);
    total = 0;
    for (i = 0; i < 12; i++) {
        PGM_P str = result[i].msg;
        char c;
        if (len > 24) {
            if (i < 10) tft.print(" ");
            tft.print(i);
            tft.print(": ");
        }
        uint8_t cnt = len;
        while ((c = pgm_read_byte(str++)) && cnt--) tft.print(c);
        tft.print(" ");
        tft.println(result[i].ms);
        total += result[i].ms;
    }
    tft.setTextSize(2);
    tft.print("Total:");
    tft.print(0.000001 * total);
    tft.println("sec");
    g_identifier = tft.readID();
    tft.print("ID: 0x");
    tft.println(tft.readID(), HEX);
//    tft.print("Reg(00):0x");
//    tft.println(tft.readReg(0x00), HEX);
    tft.print("F_CPU:");
    tft.print(0.000001 * F_CPU);
#if defined(__OPTIMIZE_SIZE__)
    tft.println("MHz -Os");
#else
    tft.println("MHz");
#endif

    delay(10000);
}

// Standard Adafruit tests.  will adjust to screen size

unsigned long testFillScreen() {
    unsigned long start = micros();
    tft.fillScreen(BLACK);
    tft.fillScreen(RED);
    tft.fillScreen(GREEN);
    tft.fillScreen(BLUE);
    tft.fillScreen(BLACK);
    Serial.println("filling complete");
    return micros() - start;
}

unsigned long testText() {
    unsigned long start;
    tft.fillScreen(BLACK);
    start = micros();
    tft.setCursor(0, 0);
    tft.setTextColor(WHITE);  tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(YELLOW); tft.setTextSize(2);
    tft.println(123.45);
    tft.setTextColor(RED);    tft.setTextSize(3);
    tft.println(0xDEADBEEF, HEX);
    tft.println();
    tft.setTextColor(GREEN);
    tft.setTextSize(5);
    tft.println("Groop");
    tft.setTextSize(2);
    tft.println("I implore thee,");
    tft.setTextSize(1);
    tft.println("my foonting turlingdromes.");
    tft.println("And hooptiously drangle me");
    tft.println("with crinkly bindlewurdles,");
    tft.println("Or I will rend thee");
    tft.println("in the gobberwarts");
    tft.println("with my blurglecruncheon,");
    tft.println("see if I don't!");
    return micros() - start;
}

unsigned long testLines(uint16_t color) {
    unsigned long start, t;
    int           x1, y1, x2, y2,
                  w = tft.width(),
                  h = tft.height();

    tft.fillScreen(BLACK);

    x1 = y1 = 0;
    y2    = h - 1;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = w - 1;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    t     = micros() - start; // fillScreen doesn't count against timing

    tft.fillScreen(BLACK);

    x1    = w - 1;
    y1    = 0;
    y2    = h - 1;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = 0;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    t    += micros() - start;

    tft.fillScreen(BLACK);

    x1    = 0;
    y1    = h - 1;
    y2    = 0;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = w - 1;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    t    += micros() - start;

    tft.fillScreen(BLACK);

    x1    = w - 1;
    y1    = h - 1;
    y2    = 0;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = 0;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

    return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
    unsigned long start;
    int           x, y, w = tft.width(), h = tft.height();

    tft.fillScreen(BLACK);
    start = micros();
    for (y = 0; y < h; y += 5) tft.drawFastHLine(0, y, w, color1);
    for (x = 0; x < w; x += 5) tft.drawFastVLine(x, 0, h, color2);

    return micros() - start;
}

unsigned long testRects(uint16_t color) {
    unsigned long start;
    int           n, i, i2,
                  cx = tft.width()  / 2,
                  cy = tft.height() / 2;

    tft.fillScreen(BLACK);
    n     = min(tft.width(), tft.height());
    start = micros();
    for (i = 2; i < n; i += 6) {
        i2 = i / 2;
        tft.drawRect(cx - i2, cy - i2, i, i, color);
    }

    return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
    unsigned long start, t = 0;
    int           n, i, i2,
                  cx = tft.width()  / 2 - 1,
                  cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    n = min(tft.width(), tft.height());
    for (i = n; i > 0; i -= 6) {
        i2    = i / 2;
        start = micros();
        tft.fillRect(cx - i2, cy - i2, i, i, color1);
        t    += micros() - start;
        // Outlines are not included in timing results
        tft.drawRect(cx - i2, cy - i2, i, i, color2);
    }

    return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
    unsigned long start;
    int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

    tft.fillScreen(BLACK);
    start = micros();
    for (x = radius; x < w; x += r2) {
        for (y = radius; y < h; y += r2) {
            tft.fillCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
    unsigned long start;
    int           x, y, r2 = radius * 2,
                        w = tft.width()  + radius,
                        h = tft.height() + radius;

    // Screen is not cleared for this one -- this is
    // intentional and does not affect the reported time.
    start = micros();
    for (x = 0; x < w; x += r2) {
        for (y = 0; y < h; y += r2) {
            tft.drawCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long testTriangles() {
    unsigned long start;
    int           n, i, cx = tft.width()  / 2 - 1,
                        cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    n     = min(cx, cy);
    start = micros();
    for (i = 0; i < n; i += 5) {
        tft.drawTriangle(
            cx    , cy - i, // peak
            cx - i, cy + i, // bottom left
            cx + i, cy + i, // bottom right
            tft.color565(0, 0, i));
    }

    return micros() - start;
}

unsigned long testFilledTriangles() {
    unsigned long start, t = 0;
    int           i, cx = tft.width()  / 2 - 1,
                     cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    start = micros();
    for (i = min(cx, cy); i > 10; i -= 5) {
        start = micros();
        tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(0, i, i));
        t += micros() - start;
        tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(i, i, 0));
    }

    return t;
}

unsigned long testRoundRects() {
    unsigned long start;
    int           w, i, i2, red, step,
                  cx = tft.width()  / 2 - 1,
                  cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    w     = min(tft.width(), tft.height());
    start = micros();
    red = 0;
    step = (256 * 6) / w;
    for (i = 0; i < w; i += 6) {
        i2 = i / 2;
        red += step;
        tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(red, 0, 0));
    }

    return micros() - start;
}

unsigned long testFilledRoundRects() {
    unsigned long start;
    int           i, i2, green, step,
                  cx = tft.width()  / 2 - 1,
                  cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    start = micros();
    green = 256;
    step = (256 * 6) / min(tft.width(), tft.height());
    for (i = min(tft.width(), tft.height()); i > 20; i -= 6) {
        i2 = i / 2;
        green -= step;
        tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, green, 0));
    }

    return micros() - start;
}
