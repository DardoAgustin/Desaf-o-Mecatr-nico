#include <SPI.h>
#include <RF24.h>
#include <ESP32Servo.h>

RF24 radio(4, 5);
const byte address[6] = "00001";

const int PIN_SERVO_X = 13;
const int PIN_SERVO_Y = 12;

Servo servoX;
Servo servoY;

struct Paquete {
  uint8_t anguloX;
  uint8_t anguloY;
};

void setup() {
  Serial.begin(115200);

  servoX.attach(PIN_SERVO_X);
  servoY.attach(PIN_SERVO_Y);
  servoX.write(90);
  servoY.write(90);

  if (!radio.begin()) {
    Serial.println("Error: NRF24L01 no detectado");
    while (true);
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX); //Potencia transceptor
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();


  Serial.println("Receptor Auto listo");
}

void loop() {
  if (radio.available()) {
    Paquete pkt;
    radio.read(&pkt, sizeof(pkt)); //Lee el paquete enviado desde el control

    servoX.write(constrain(pkt.anguloX, 0, 180));
    servoY.write(constrain(pkt.anguloY, 0, 180));

    Serial.print("X: "); Serial.print(pkt.anguloX);
    Serial.print("°  Y: "); Serial.print(pkt.anguloY);
    Serial.println("°");
  }
}