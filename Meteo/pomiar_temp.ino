#define dht_dpin A0


struct Data {
  double humidity;
  double temperature;
  int status;
} data;

#define BYTES_NUMBER 5
#define BITS_NUMBER BYTES_NUMBER*5
#define REQUEST_LOW 20
#define REQUEST_HIGH 40 // microsec!
#define TIMEOUT 10000

Data read(uint8_t pin)
{
  uint8_t bits[BYTES_NUMBER] = {0}; 
  uint8_t cnt = 7;
  uint8_t idx = 0;

  struct Data tempData;
  tempData.status = 0

  // MC wysyla zapytanie o dane do czujnika
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(REQUEST_LOW);
  digitalWrite(pin, HIGH);
  delayMicroseconds(REQUEST_HIGH);
  pinMode(pin, INPUT);

  // nasluchujemy na odpowiedz od czujnika
  unsigned int loopCnt = TIMEOUT; // TODO: hack
  while(digitalRead(pin) == LOW)
    if (loopCnt-- == 0) tempData.status = -2;

  loopCnt = TIMEOUT;
  while(digitalRead(pin) == HIGH)
    if (loopCnt-- == 0) tempData.status = -2;

  // zaczynamy odbierac dane  
  for (int i=0; i<BITS_NUMBER; i++)
  {
    loopCnt = TIMEOUT;
    while(digitalRead(pin) == LOW)
      if (loopCnt-- == 0) tempData.status = -2;

    unsigned long t = micros();

    loopCnt = TIMEOUT;
    while(digitalRead(pin) == HIGH)
      if (loopCnt-- == 0) tempData.status = -2;

    if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
    if (cnt == 0)   // next byte?
    {
      cnt = 7;   
      idx++;      
    }
    else cnt--;
  }

  // CONVERT AND STORE
  tempData.humidity    = bits[0];  // bit[1] == 0;
  tempData.temperature = bits[2];  // bits[3] == 0;

  // TEST CHECKSUM
  uint8_t sum = bits[0] + bits[2]; // bits[1] && bits[3] both 0
  if (bits[4] != sum) tempData.status = -1;

  return tempData;
}


void setup(){
  Serial.begin(9600);
  delay(300);
  Serial.println("Wilgotnosc i temperatura:\n\n");
  delay(700);     //Czekamy polecana ilosc czasu (dopelnienie do 1000ms)
}

void loop(){

  struct Data tempData = read(dht_dpin);; 

    Serial.print("Wilgotnosc = ");
    Serial.print(tempData.humidity);
    Serial.print("%  ");
    Serial.print("temperatura = ");
    Serial.print(tempData.temperature); 
    Serial.println("C  ");
  delay(1000);
}

