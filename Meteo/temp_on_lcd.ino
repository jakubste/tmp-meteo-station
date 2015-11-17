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


// struct `DHTData` is used to store DHTData received from DHT11
struct DHTData {
    double humidity;
    double temperature;
    int status;
};

struct DHTData read(uint8_t pin) {
    uint8_t bytes[BYTES_NUMBER] = {0};
    // cnt and idx indicate position to store received bit in bytes table
    uint8_t cnt = 7; // bit position in current byte
    uint8_t idx = 0; // current byte

    struct DHTData tempDHTData;
    tempDHTData.status = 0; // 0 is good, if we fail we'd change this to negative number

    // we're sending request for DHTData to DHT11
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
        if (t + TIMEOUT < micros()) tempDHTData.status = -2;

    t = micros();
    while (digitalRead(pin) == HIGH)
        if (t + TIMEOUT < micros()) tempDHTData.status = -2;

    // receiving DHTData
    for (int i = 0; i < BITS_NUMBER; i++) {
        t = micros();
        while (digitalRead(pin) == LOW)
            if (t + TIMEOUT < micros()) tempDHTData.status = -2;

        t = micros();
        while (digitalRead(pin) == HIGH)
            if (t + TIMEOUT < micros()) tempDHTData.status = -2;

        if ((micros() - t) > 40) bytes[idx] |= (1 << cnt);
        if (cnt == 0)   // next byte?
        {
            cnt = 7;
            idx++;
        }
        else cnt--;
    }

    // CONVERT AND STORE
    tempDHTData.humidity = bytes[0];  // bytes[1] == 0;
    tempDHTData.temperature = bytes[2];  // bytes[3] == 0;

    // TEST CHECKSUM
    uint8_t sum = bytes[0] + bytes[2]; // bytes[1] && bytes[3] both 0
    if (bytes[4] != sum) tempDHTData.status = -1;

    return tempDHTData;
}

void setup(void) {

    Serial.begin(9600);

#ifdef DEBUG
    Serial.println(F("Stacja Meteo"));
#endif // DEBUG

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

    tft.fillScreen(BLACK);
    tft.setRotation(0); //kablem USB na dol

    tft.setTextColor(WHITE);
    tft.setTextSize(2);

    tft.setCursor(10, 100);

    tft.println("Temperatura:");

    tft.setCursor(10, 150);

    tft.println("Cisnienie:");

    /*
    tft.fillRect(120, 240, 60, 60, YELLOW);
    tft.setTextColor(BLACK);
    tft.setCursor(140, 260);
    tft.println(">>");
    */

    tft.setTextColor(WHITE);


}


void loop() {

    struct DHTData tempDHTData = read(dht_dpin);;
    delay(1000);

    Serial.print("Wilgotnosc = ");
    Serial.print(tempDHTData.humidity);
    Serial.print("%  ");
    Serial.print("temperatura = ");
    Serial.print(tempDHTData.temperature);
    Serial.println("C  ");
 
    tft.fillRect(169, 99, 30, 30, BLACK);

    tft.setCursor(170, 100);

    tft.print((int)tempDHTData.temperature);
    tft.print(" C");

    tft.fillRect(169, 149, 30, 30, BLACK);
    tft.setCursor(170, 150);

    tft.print((int)tempDHTData.humidity);
    tft.println(" %");

}


