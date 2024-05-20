/*
*
* RDID - ESP32 PUERTA
*
*/



#include <SPI.h>
#include <MFRC522.h> // https://github.com/miguelbalboa/rfid

#define PIN_SDA 10 // PIN SDA
#define PIN_RST 9 // PIN RESET
#define PIN_RELE 3
#define PIN_LED 6

bool inicializada = false;
byte UID_AUTORIZADA[4];
byte UID_RECIBIDA[4];

MFRC522 lector_rfid(PIN_SDA, PIN_RST);

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);
  /*
  * Inicio de los modulos del rfid
  */
  SPI.begin();
  lector_rfid.PCD_Init();   //REGISTRAR TARJETA
  delay(10);
  lector_rfid.PCD_DumpVersionToSerial(); // Muestra la informacion del lector RFID si hay serial.
}

void loop() {
  while (inicializada == false) {   // HASTA QUE SE INICIALICE BUCLE
    digitalWrite(PIN_LED, HIGH);    // LED PARPADEANDO
    delay(100);
    if (lector_rfid.PICC_IsNewCardPresent()) {  // HAY TARJETA YA REGISTRADA?

      if (lector_rfid.PICC_ReadCardSerial()) {  // COMPRONAR QUE ES CORRECTA
        // ORDENES DE LECTURA 
        Serial.print("UID Recibida: ");
        for (int i = 0;i < 4;i++) {
          UID_AUTORIZADA[i] = lector_rfid.uid.uidByte[i];
          Serial.print(UID_AUTORIZADA[i], HEX);
          Serial.print(" ");
        }
        Serial.println(" ");
        digitalWrite(PIN_LED, HIGH);    // LED SE MANTIENE 3 SEGUNDOS ENCENDIDA
        delay(3000);
        inicializada = true;            // YA HAY TARJETA INICIALIZADA
      }

      else {
        Serial.println("ERR. TARJETA RFID NO VALIDA");    // NO CORRECTA Y SIGUE PITANDO Y COMPROBANDO
      }
    }
    digitalWrite(PIN_LED, LOW);   // LED SE APAGA
    delay(100);
  }

  if (lector_rfid.PICC_IsNewCardPresent()) {    //SI YA HA SIDO INICIALIZADA REALMENTE

      if (lector_rfid.PICC_ReadCardSerial()) {    // LEE LA TARJETA CON LA QUE SE ESTÉ INTENTANDO ABRIR LA PUERTA
        // ORDENES DE LECTURA 
        Serial.print("UID Recibida: ");
        for (int i = 0;i < 4;i++) {
          UID_RECIBIDA[i] = lector_rfid.uid.uidByte[i];
          Serial.print(UID_RECIBIDA[i], HEX);
          Serial.print(" ");
        }
        Serial.println(" ");

        // Verifica si esa UID es la misma que la autorizada.
        if (UID_RECIBIDA[0] == UID_AUTORIZADA[0] &&     // SI LA RECIBIDA == AUTORIZADA 
          UID_RECIBIDA[1] == UID_AUTORIZADA[1] &&
          UID_RECIBIDA[2] == UID_AUTORIZADA[2] &&
          UID_RECIBIDA[3] == UID_AUTORIZADA[3]) {
          Serial.println("AUTORIZADO, ABRIENDO");
          digitalWrite(PIN_RELE, HIGH);                 // SE ACTIVA EL RELE
          digitalWrite(PIN_LED, HIGH);                  // LED SE MANTIENE ENCENDIDA 1'5 SEG
          delay(1500);
          digitalWrite(PIN_RELE, LOW);                  // SE APAGA EL RELE
          digitalWrite(PIN_LED, LOW);                   // LED SE APAGA
        }

        else {
          Serial.println("NO AUTORIZADO");      // RECIBIDA != AUTORIZADA
          for (int i = 0;i < 3;i++) {
            digitalWrite(PIN_LED, HIGH);        // 2 PITIDOS DE LED RAPIDOS
            delay(50);
            digitalWrite(PIN_LED, LOW);
            delay(50);
          }
        }
        lector_rfid.PICC_HaltA(); //Pone el lector en modo SLEEP hasta que se acerque otra tajeta

      }
      else {
          Serial.println("ERR. TARJETA RFID NO VALIDA");    // LA TARJETA RECIBIDA NO ES VÁLIDA
      }

  }
}