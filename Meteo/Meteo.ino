// LCD pins:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

#define DEBUG 1

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4


#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 110
#define TS_MAXX 890
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
//#define ROZ     0xFD20
#define ROZ     0xFBE0
#define GRI     0xBDF7
// http://stackoverflow.com/questions/13720937/c-defined-16bit-high-color
// http://wiibrew.org/wiki/U16_colors

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);;

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;
int ics;


String data[2][2] = {"10", "20", "30", "40"};

int cur = 0;


void setup(void) {
#ifdef DEBUG
    Serial.begin(9600);
    Serial.println(F("Stacja Meteo"));
#endif // DEBUG

    tft.reset();

    uint16_t identifier = tft.readID();

    if (identifier == 0x9341) {
#ifdef DEBUG
        Serial.println('Ekran zidentyfikowany poprawnie');
#endif // DEBUG
    } else {
#ifdef DEBUG
        Serial.println('[error] Ekran zidentyfikowany błędnie!');
#endif // DEBUG
        identifier = 0x9341;
    }


    tft.begin(identifier);

    tft.fillScreen(BLACK);
    tft.setRotation(0); //kablem USB na dol

    tft.setTextColor(WHITE);
    tft.setTextSize(2);

    tft.setCursor(10, 100);

    tft.println("Temperatura:");

    tft.setCursor(10, 150);

    tft.println("Cisnienie:");

    tft.fillRect(120, 240, 60, 60, YELLOW);
    tft.setTextColor(BLACK);
    tft.setCursor(140, 260);
    tft.println(">>");

    tft.setTextColor(WHITE);

    tft.fillRect(170, 100, 20, 20, BLACK);

    tft.setCursor(170, 100);

    tft.println(data[cur][0]);

    tft.fillRect(170, 150, 20, 20, BLACK);
    tft.setCursor(170, 150);

    tft.println(data[cur][1]);

    cur = cur + 1;
    cur = cur % 2;


}

#define MINPRESSURE 10
#define MAXPRESSURE 1000


void loop() {
    digitalWrite(13, HIGH);
    // Recently Point was renamed TSPoint in the TouchScreen library
    // If you are using an older version of the library, use the
    // commented definition instead.
    // Point p = ts.getPoint();
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    //pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    //pinMode(YM, OUTPUT);

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
#ifdef DEBUG
        Serial.print("X = ");
        Serial.print(p.x);
        Serial.print("\tY = ");
        Serial.print(p.y);
        Serial.print("\tPressure = ");
        Serial.println(p.z);
#endif // DEBUG
        if (p.y < (TS_MINY - 5)) stergere();
        // scale from 0->1023 to tft.width
        p.x = tft.width() - (map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
        p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
#ifdef DEBUG
        Serial.print("(");
        Serial.print(p.x);
        Serial.print(", ");
        Serial.print(p.y);
        Serial.println(")");
#endif // DEBUG

        if (120 < p.x && p.x < 180 && 240 < p.y && p.y < 300) {
#ifdef DEBUG
            Serial.println("Klik");
            delay(100);
#endif

            tft.fillRect(170, 100, 20, 20, BLACK);

            tft.setCursor(170, 100);

            tft.println(data[cur][0]);

            tft.fillRect(170, 150, 20, 20, BLACK);
            tft.setCursor(170, 150);

            tft.println(data[cur][1]);

            cur = cur + 1;
            cur = cur % 2;

        }
    }
}

void text(int culoare) {
    tft.setRotation(3);

    tft.setCursor(5, 220);
    tft.setTextSize(2);
    tft.println("BUHOSOFT");
    tft.setRotation(0);
}

void cifre(int culoare) {
// number for "buttons"
    tft.setRotation(3);
    tft.setTextColor(culoare);
    tft.setTextSize(3);
    tft.drawLine(0, 0, 40, 40, culoare);
    tft.drawLine(0, 40, 40, 0, culoare);
// tft.setCursor(15, 10);
// tft.println("1");
    tft.setCursor(55, 10);
    tft.println("1");
    tft.setCursor(95, 10);
    tft.println("2");
    tft.setCursor(135, 10);
    tft.println("3");
    tft.setCursor(175, 10);
    tft.println("4");
    tft.setCursor(215, 10);
    tft.println("5");
    tft.setCursor(255, 10);
    tft.println("6");
    tft.setCursor(295, 10);
    tft.println("7");

    tft.setRotation(0);
}

void stergere() {
#ifdef DEBUG
    Serial.println("erase");
#endif // DEBUG
    // press the bottom of the screen to erase
    //     tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, BLACK);
    tft.fillRect(0, 0, 240, 320, BLACK);
    tft.setRotation(0);
    tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
    tft.fillRect(0, BOXSIZE, BOXSIZE, BOXSIZE, YELLOW);
    tft.fillRect(0, BOXSIZE * 2, BOXSIZE, BOXSIZE, GREEN);
    tft.fillRect(0, BOXSIZE * 3, BOXSIZE, BOXSIZE, CYAN);
    tft.fillRect(0, BOXSIZE * 4, BOXSIZE, BOXSIZE, BLUE);
    tft.fillRect(0, BOXSIZE * 5, BOXSIZE, BOXSIZE, MAGENTA);
    tft.fillRect(0, BOXSIZE * 6, BOXSIZE, BOXSIZE, GRI);
    tft.fillRect(0, BOXSIZE * 7, BOXSIZE, BOXSIZE, ROZ);
//tft.fillRect(BOXSIZE, BOXSIZE, BOXSIZE, BOXSIZE, WHITE);

    tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
    currentcolor = RED;
    cifre(WHITE);
}

