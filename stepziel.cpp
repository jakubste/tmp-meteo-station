#define dht_dpin A0 //no ; here. Set equal to channel sensor is on
#define TIMEOUT 10000

struct Data {
  double humidity;
  double temperature;
  int status;
} data;

void setup(){
  Serial.begin(9600);
  delay(300);
  Serial.println("Wilgotnosc i temperatura:\n\n");
  delay(700);     //Czekamy polecana ilosc czasu (dopelnienie do 1000ms)
}

void loop(){

  struct Data tempData = readDHT11(dht_dpin);; 

    Serial.print("Wilgotnosc = ");
    Serial.print(tempData.humidity);
    Serial.print("%  ");
    Serial.print("temperatura = ");
    Serial.print(tempData.temperature); 
    Serial.println("C  ");
  delay(1000);
}

Data readDHT11(uint8_t pin)
{
  // READ VALUES

  struct Data tempData;

  int rv = read(pin);
  if (rv != 0) return rv;

  // CONVERT AND STORE
  tempData.humidity    = bits[0];  // bit[1] == 0;
  tempData.temperature = bits[2];  // bits[3] == 0;

  // TEST CHECKSUM
  uint8_t sum = bits[0] + bits[2]; // bits[1] && bits[3] both 0
  if (bits[4] != sum) return -1;

  return tempData;
}

Data read(uint8_t pin)
{
  // INIT BUFFERVAR TO RECEIVE DATA
  uint8_t bits[5]; 
  uint8_t cnt = 7;
  uint8_t idx = 0;

  // EMPTY BUFFER
  for (int i=0; i< 5; i++) bits[i] = 0;

  // REQUEST SAMPLE
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(20);
  digitalWrite(pin, HIGH);
  delayMicroseconds(40);
  pinMode(pin, INPUT);

  // GET ACKNOWLEDGE or TIMEOUT
  unsigned int loopCnt = TIMEOUT;
  while(digitalRead(pin) == LOW)
    if (loopCnt-- == 0) return -2;

  loopCnt = TIMEOUT;
  while(digitalRead(pin) == HIGH)
    if (loopCnt-- == 0) return -2;

  // READ THE OUTPUT - 40 BITS => 5 BYTES
  for (int i=0; i<40; i++)
  {
    loopCnt = TIMEOUT;
    while(digitalRead(pin) == LOW)
      if (loopCnt-- == 0) return -2;

    unsigned long t = micros();

    loopCnt = TIMEOUT;
    while(digitalRead(pin) == HIGH)
      if (loopCnt-- == 0) return -2;

    if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
    if (cnt == 0)   // next byte?
    {
      cnt = 7;   
      idx++;      
    }
    else cnt--;
  }


  struct Data tempData;

  int rv = read(pin);
  if (rv != 0) return rv;

  // CONVERT AND STORE
  tempData.humidity    = bits[0];  // bit[1] == 0;
  tempData.temperature = bits[2];  // bits[3] == 0;

  // TEST CHECKSUM
  uint8_t sum = bits[0] + bits[2]; // bits[1] && bits[3] both 0
  if (bits[4] != sum) return -1;

  return tempData;
}