// LCD pins:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

//#define DEBUG 1

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Wire.h>
#include "RTClib.h"

/*** PINS DEFINITION ***/

// LCD:
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4 //optional
// Touchscreen pins
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin
// DHT11
#define dht_dpin 30


/*** CONSTS DEFINITION ***/
// Touchscreen reference
#define TS_MINX 150
#define TS_MINY 110
#define TS_MAXX 890
#define TS_MAXY 920
#define MINPRESSURE 10 // minimum pressire that we take into consideration
#define MAXPRESSURE 1000 // maximum --//--

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
#define ROZ     0xFBE0
#define GRI     0xBDF7
// http://stackoverflow.com/questions/13720937/c-defined-16bit-high-color
// http://wiibrew.org/wiki/U16_colors

// DHT11 constants
#define BYTES_NUMBER 5
#define BITS_NUMBER BYTES_NUMBER*5
#define REQUEST_LOW 20
#define REQUEST_HIGH 40 // microsec!
#define TIMEOUT 10000


// lcd object declarations
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


// RTC
RTC_DS1307 RTC;


// struct `Data` is used to store data received from DHT11
struct Data {
    double humidity;
    double temperature;
    int status;
} data;


Data read(uint8_t pin) {
    uint8_t bytes[BYTES_NUMBER] = {0};
    // cnt and idx indicate position to store received bit in bytes table
    uint8_t cnt = 7; // bit position in current byte
    uint8_t idx = 0; // current byte

    struct Data tempData;
    tempData.status = 0; // 0 is good, if we fail we'd change this to negative number

    // we're sending request for data to DHT11
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(REQUEST_LOW); // low signal for 20 ms
    digitalWrite(pin, HIGH);
    delayMicroseconds(REQUEST_HIGH); // high signal for 40 us
    pinMode(pin, INPUT);  // pull-up resistor is applyed, we're listening

    unsigned long t;

    // we're listening DHT11's start signal
    t = micros();
    while (digitalRead(pin) == LOW)
        if (t + TIMEOUT < micros()) tempData.status = -2;

    t = micros();
    while (digitalRead(pin) == HIGH)
        if (t + TIMEOUT < micros()) tempData.status = -2;

    // receiving data
    for (int i = 0; i < BITS_NUMBER; i++) {
        t = micros();
        while (digitalRead(pin) == LOW)
            if (t + TIMEOUT < micros()) tempData.status = -2;

        t = micros();
        while (digitalRead(pin) == HIGH)
            if (t + TIMEOUT < micros()) tempData.status = -2;

        if ((micros() - t) > 40) bytes[idx] |= (1 << cnt);
        if (cnt == 0)   // next byte?
        {
            cnt = 7;
            idx++;
        }
        else cnt--;
    }

    // CONVERT AND STORE
    tempData.humidity = bytes[0];  // bytes[1] == 0;
    tempData.temperature = bytes[2];  // bytes[3] == 0;

    // TEST CHECKSUM
    uint8_t sum = bytes[0] + bytes[2]; // bytes[1] && bytes[3] both 0
    if (bytes[4] != sum) tempData.status = -1;

    return tempData;
}

Data old_data;
DateTime old_time;

void setup(void) {

    Serial.begin(9600);

#ifdef DEBUG
    Serial.println(F("Stacja Meteo"));
#endif
    tft.reset();

    uint16_t identifier = tft.readID();

    if (identifier == 0x9341) {
#ifdef DEBUG
        Serial.println("Ekran zidentyfikowany poprawnie");
#endif // DEBUG
    } else {
#ifdef DEBUG
        Serial.println("[error] Ekran zidentyfikowany błędnie!");
#endif // DEBUG
        identifier = 0x9341;
    }

    tft.begin(identifier);

    tft.setRotation(0); //kablem USB na dol

    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);

    tft.fillRect(120, 240, 60, 60, YELLOW);
    tft.setTextColor(BLACK);
    tft.setCursor(140, 260);
    tft.println(">>");
    
    tft.setTextColor(WHITE);

    Wire.begin();
    RTC.begin();

    RTC.adjust(DateTime(__DATE__, __TIME__));
    //RTC.adjust(DateTime(2015, 12, 31, 23, 59, 45));


    old_data = read(dht_dpin);
    old_time = RTC.now();
    delay(500);

}

char state = 't'; 
bool first = true;

void loop() {
        
    
    if (state == 't'){
      
        struct Data tempData = read(dht_dpin);
      
        delay(500);

        #ifdef DEBUG
            Serial.print("Wilgotnosc = ");
            Serial.print((int)tempData.humidity);
            Serial.print("%  ");
            Serial.print("temperatura = ");
            Serial.print((int)tempData.temperature);
            Serial.println("C  ");
        #endif

        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        
        if (first || (int)tempData.temperature != (int)old_data.temperature) {
            tft.setCursor(10, 100);
            tft.println("Temperatura:");
            tft.fillRect(165, 95, 30, 30, BLACK);
            tft.setCursor(170, 100);
            tft.print((int)tempData.temperature);
            tft.print(" C");
        }
        
        if (first || (int)tempData.humidity != (int)old_data.humidity) {
            tft.setCursor(10, 150);
            tft.println("Wilgotnosc:");
            tft.fillRect(165, 145, 30, 30, BLACK);
            tft.setCursor(170, 150);
            tft.print((int)tempData.humidity);
            tft.println(" %");
        }
        old_data = tempData;
        first = false;
    }
    else {
        DateTime now = RTC.now();

        #ifdef DEBUG
            if (now.hour() < 10) Serial.print('0'); 
            Serial.print(now.hour());
            Serial.print(':');
            if (now.minute() < 10) Serial.print('0');
            Serial.print(now.minute());
            Serial.print(':');
            if (now.second() < 10) Serial.print('0');
            Serial.println(now.second());
        #endif

        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        
        if (first) {
            tft.fillRect(10, 50, 250, 50, BLACK);
            first = false;
            tft.setCursor(30, 50);
            if (now.hour() < 10) tft.print('0'); 
            tft.print(now.hour());
            tft.print(':');
            if (now.minute() < 10) tft.print('0');
            tft.print(now.minute());
            tft.print(':');
            if (now.second() < 10) tft.print('0');
            tft.print(now.second());
         

            tft.setTextSize(3);
            tft.fillRect(10, 100, 250, 50, BLACK);
            first = false;
            tft.setCursor(35, 100);
            if (now.day() < 10) tft.print('0'); 
            tft.print(now.day());
            tft.print('/');
            if (now.month() < 10) tft.print('0');
            tft.print(now.month());
            tft.print('/');
            tft.print(now.year());

            first = false;
        }

        if (now.hour() != old_time.hour()) {
            tft.setTextSize(4);
            tft.fillRect(25, 45, 55, 40, BLACK);
            tft.setCursor(30, 50);
            if (now.hour() < 10) tft.print('0'); 
            tft.print(now.hour());
        }

        if (now.minute() != old_time.minute()) {
            tft.setTextSize(4);
            tft.fillRect(95, 45, 55, 40, BLACK);
            tft.setCursor(100, 50);
            if (now.minute() < 10) tft.print('0');
            tft.print(now.minute());
        }

        if (now.second() != old_time.second()) {
            tft.setTextSize(4);
            tft.fillRect(170, 45, 55, 40, BLACK);
            tft.setCursor(173, 50);
            if (now.second() < 10) tft.print('0');
            tft.print(now.second());
        }

        if (now.day() != old_time.day()) {
            tft.setTextSize(3);
            tft.fillRect(30, 95, 40, 40, BLACK);
            tft.setCursor(35, 100);
            if (now.day() < 10) tft.print('0');
            tft.print(now.day());
        }

        if (now.month() != old_time.month()) {
            tft.setTextSize(3);
            tft.fillRect(85, 95, 40, 40, BLACK);
            tft.setCursor(90, 100);
            if (now.month() < 10) tft.print('0');
            tft.print(now.month());
        }

        if (now.year() != old_time.year()) {
            tft.setTextSize(3);
            tft.fillRect(140, 95, 80, 40, BLACK);
            tft.setCursor(145, 100);
            tft.print(now.year());
        }

        old_time = now;
        delay(300);
    }
  
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
        //if (p.y < (TS_MINY - 5)) stergere();
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

            if (state == 't') {
                state = 'c';
                first = true;
            } else {
                state = 't';
                first = true;
            }
            tft.fillScreen(BLACK);
            tft.fillRect(120, 240, 60, 60, YELLOW);
            tft.setTextColor(BLACK);
            tft.setTextSize(2);
            tft.setCursor(140, 260);
            tft.println(">>");
            

        }
    }
}

