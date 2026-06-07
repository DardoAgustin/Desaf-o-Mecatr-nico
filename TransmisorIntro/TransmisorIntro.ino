#include <SPI.h>
#include <RF24.h>

RF24 radio(4, 5);
const byte address[6] = "00001";
const int DEAD_ZONE = 250;
const int MIDPOINT  = 2047;
const int PIN_JOY_X = 12; //Eje X (Dirección delantera)
const int PIN_JOY_Y = 13; // Eje Y (Apertura de tanque)

struct Paquete {
  uint8_t anguloX; // 0 a 180
  uint8_t anguloY; // 0 a 180
};

void setup() {
  Serial.begin(115200);

  if (!radio.begin()) {
    Serial.println("Error: NRF24L01 no detectado");
    while (true);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX); //Potencia transceptor
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();

  Serial.println("Transmisor joystick listo"); //Msj de evento
}

void loop() {
  //Lectura de joystick sin procesar
  int rawX = analogRead(PIN_JOY_X);
  int rawY = analogRead(PIN_JOY_Y);
  //
  Paquete pkt;
  //procesamiento de los datos más integración de un punto muerto en el Joystick
  pkt.anguloX = 90;
  if(rawX < (MIDPOINT-DEAD_ZONE))
  pkt.anguloX = map(rawX, 0, (MIDPOINT-DEAD_ZONE), 0, 89);
  else if(rawX > (MIDPOINT+DEAD_ZONE))
  pkt.anguloX = map(rawX, 4095, (MIDPOINT+DEAD_ZONE), 180, 91);
  pkt.anguloY = 90;
  if(rawY < (MIDPOINT-DEAD_ZONE))
  pkt.anguloY = map(rawY, 0, (MIDPOINT-DEAD_ZONE), 0, 89);
  else if(rawY > (MIDPOINT+DEAD_ZONE))
  pkt.anguloY = map(rawY, 4095, (MIDPOINT+DEAD_ZONE), 180, 91);

  bool ok = radio.write(&pkt, sizeof(pkt));

  Serial.print("X: "); Serial.print(pkt.anguloX);Serial.print("|");Serial.print(rawX);
  Serial.print("°  Y: "); Serial.print(pkt.anguloY);Serial.print("|");Serial.print(rawY);
  Serial.println(ok ? "°  [OK]" : "°  [FALLO]");

  delay(20);
}