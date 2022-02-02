#include <SoftwareSerial.h>

#define RX_485 6
#define TX_485 7
#define RX_GSM 3
#define TX_GSM 2

SoftwareSerial rs485(RX_485, TX_485);
SoftwareSerial gsm(RX_GSM, TX_GSM);

byte readDistance [8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};
byte ultrasonic_data [7];
byte readLevel [8] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0B};

String apn = "default";
String url = "http://m.and-water.uz/bot/app.php";

uint16_t distance;
String data;
String id = "0026";

void setup() {
  Serial.begin(9600);
  rs485.begin(9600);
  gsm.begin(9600);
}

unsigned long cur_tim = 0;

void loop() {
  get_waterlevel();
}

void get_waterlevel() {
  rs485.listen();
  distance = 0;
  if (int k = rs485.available()) {
    rs485.readBytes(ultrasonic_data, 7);
    distance = ultrasonic_data[3] << 8 | ultrasonic_data[4];
    Serial.print("data: ");
    Serial.println(distance, DEC);
    gsmConnection();
  }
  if (millis() - cur_tim > 1000) {
    cur_tim = millis();
    delay(50);
    rs485.write(readLevel, 8);
    delay(50);
  }
}
void gsmConnection() {
  gsm.listen();
  gsm.println("AT");
  readGsm();
  delay(2000);

  gsm.println("AT+CSQ"); // get gsm signal
  readGsm();
  delay(2000);
  
  gsm.println("AT+SAPBR=3,1,Contype,GPRS");
  readGsm();
  delay(3000);
  gsm.println("AT+SAPBR=3,1,APN," + apn);
  readGsm();
  delay(3000);

  gsm.println("AT+SAPBR=1,1");
  readGsm();
  delay(2000);
  gsm.println("AT+SAPBR=2,1");
  readGsm();
  delay(4000);

  gsm.println("AT+HTTPINIT");
  readGsm();
  delay(3000);
  gsm.println("AT+HTTPPARA=CID,1");
  readGsm();
  delay(2000);
  
  data = "";
  data = "?id=" + id + "&data=" + distance;
  gsm.println("AT+HTTPPARA=URL," + url + data);
  readGsm();
  delay(4000);
  Serial.print("Sended data: ");
  Serial.println(distance);
  gsm.println("AT+HTTPACTION=1");
  readGsm();
  delay(6000);
  gsm.println("AT+HTTPREAD");
  readGsm();
  delay(2000);
  gsm.println("AT+HTTPTERM");
  readGsm();
  delay(3000);
  gsm.println("AT+SAPBR=0,1");
  delay(3000);
}

void readGsm() {
  while (gsm.available()) {
    Serial.write(gsm.read());
  }
}
