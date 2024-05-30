#include <Servo.h>

/*
* CONFIGURACION DE LA DISTRIBUCION DE PINES
*/
#define sensorIzquierdo 2
#define sensorDerecho 3
#define sensorTrasero 4
#define boton 7 //pin 4 y pin negativo
#define motorIzquierdo 6
#define motorDerecho 5

#define pin_echo_sensor1 8  //Sensor ultrasonido pin 8
#define pin_trigger_sensor1 9  //Sensor ultrasonido pin 9

#define pin_echo_sensor2 10     //Sensor ultrasonidos pin 10
#define pin_trigger_sensor2 11  //Sensor ultrasonidos pin 11

#define pin_echo_sensor3 12  //Sensor ultrasonidos pin 12
#define pin_trigger_sensor3 13  //Sensor ultrasonidos pin 13

#define distanciaMaxima 20        // Esta será la distancia de detección del robot


/*
*ESTE CODIGO ES PARA INICIALIZAR LAS LIBRERIAS
*/
Servo servoDerecho;   //Se inicia un servo con el nombre servoDerecho
Servo servoIzquierdo; //Se inicia un servo con el nombre servoIzquierdo




/*
*DECLARAMOS UNA SERIE DE VARIABLES PARA HACER EL PROGRAMA MAS LEGIBLE Y PRÁCTICO
*/
boolean modoCombate = false;  // Inicializamos la variable para que el robot comience parado
int estadoSensorDerecho;    //Variable que almacena el estado del sensor Derecho
int estadoSensorIzquierdo;  //Variable que almacena el estado del sensor Izquierdo
int estadoSensorTrasero; //Variable que almacena el estado del sensor Trasero

long distancia1;
long distancia2;
long distancia3;

/*
*DECLARAMOS LOS MÉTODOS QUE VAMOS A UTILIZAR
*/
void leerBoton();
void leerSensores();
long ultrasonidos(int pin_trig_sensor,int pin_echo_sensor);
void girarDerecha();
void girarIzquierda();
void avanza();
void atras();
void parado();
void evasion();



void setup() {
  pinMode(boton, INPUT_PULLUP); //El boton sera una entrada
  pinMode(sensorDerecho, INPUT);  //El pin sera una entrada para leer el sensor de linea
  pinMode(sensorIzquierdo, INPUT);//El pin sera una entrada para leer el sensor de linea
  pinMode(pin_echo_sensor1, INPUT);     //El pin sera una entrada para leer el sensor de ultrasonidos
  pinMode(pin_trigger_sensor1, OUTPUT); //El pin enviara señales ultrasónicas, será una salida.
  pinMode(pin_echo_sensor2, INPUT);     //El pin sera una entrada para leer el sensor de ultrasonidos
  pinMode(pin_trigger_sensor2, OUTPUT); //El pin enviara señales ultrasónicas, será una salida.
  pinMode(pin_echo_sensor3, INPUT);     //El pin sera una entrada para leer el sensor de ultrasonidos
  pinMode(pin_trigger_sensor3, OUTPUT); //El pin enviara señales ultrasónicas, será una salida.
  
  servoDerecho.attach(motorDerecho);      //El pin va a manejar un servo
  servoIzquierdo.attach(motorIzquierdo);  //El pin va a manejar un servo

  //Al conectar el robot al ordenador por USB recibimos mensajes de diagnóstico
  Serial.begin(9600);
  delay(10000);
  Serial.println("ROBOT PREPARADO...");
  Serial.println("Presione el boton para comenzar el combate");
}


void loop() {
  delay(50);      //Hacemos una pequeña pausa para estabilizar el programa
  leerBoton();    //Leemos el estado del boton, para arrancar o parar el robot
  leerSensores(); //Leemos los sensores de ultrasonidos y de linea

  if (modoCombate == false) { //EL ROBOT ESTA EN ESPERA
    parado();
  }
  else { //EL ROBOT ESTA EN MODO COMBATE
    if(estadoSensorTrasero == LOW) {

      if (estadoSensorTrasero == LOW && estadoSensorIzquierdo == LOW) { //Detecta linea con los sensores izquierdo y trasero
        girarDerecha(); //Esquiva la linea
      } 
      else if (estadoSensorTrasero == LOW && estadoSensorDerecho == LOW) { //Detecta linea con los sensores derecho y trasero
        girarIzquierda(); //Esquiva la linea
      }
      adelante();
    }
    
    else if (estadoSensorDerecho == LOW || estadoSensorIzquierdo == LOW) { //Detecta lineacon los sensores delanteros
      evasion();  //Esquiva la linea
    }

    else if (distancia1 <= distanciaMaxima ) { //DETECTA UN ENEMIGO  
      adelante(); //Ataca moviendose hacia delante
    }

    else if (distancia2 <= distanciaMaxima ) { //DETECTA UN ENEMIGO
      girarIzquierda();
    }

    else if (distancia3 <= distanciaMaxima ) { //DETECTA UN ENEMIGO
      girarDerecha();
    }

    else {
      girarDerecha();//BUSCA AL ENEMIGO
    }
  }

}
/*
*ESTA FUNCION LEE EL BOTON DEL ROBOT
*/
void leerBoton() {
  int estadoBoton = digitalRead(boton);  //lee el estado del boton

  if (estadoBoton == LOW) {
    modoCombate = !modoCombate; //CAMBIAMOS EL ESTADO DEL ROBOT AL PULSAR EL BOTON
    delay(500); //delay antirebote, para que el boton no mande distintas señales
    if (modoCombate == true){ //ANTES DE ENTRAR EN COMBATE ESPERAMOS 5 SEGUNDOS
      Serial.println("ENTRANDO EN MODO COMBATE");
      delay(4500);
    }
    if(modoCombate == false){
      Serial.println("ENTRANDO EN MODO REPOSO");
    }
  }
}
/*
*ESTA FUNCION LEE LOS SENSORES DEL ROBOT
*/
void leerSensores() {
  estadoSensorDerecho = digitalRead(sensorDerecho); //Lee el sensor de linea derecho
  estadoSensorIzquierdo = digitalRead(sensorIzquierdo); //Lee el sensor de linea izquierdo
  estadoSensorTrasero = digitalRead(sensorTrasero); //Lee el sensor de linea trasero
  
  //Lee los sensores de ultrasonidos y convierte la informacion a centimetros para poder medir la distancia
  Serial.print("DF: ");
  distancia1 = ultrasonidos(pin_trigger_sensor1, pin_echo_sensor1);

  Serial.print("DI: ");
  distancia2 = ultrasonidos(pin_trigger_sensor2, pin_echo_sensor2);

  Serial.print("DD: ");
  distancia3 = ultrasonidos(pin_trigger_sensor3, pin_echo_sensor3);
}
/*
*ESTA FUNCION INICIALILZA Y ACTIVA UN SENSOR ULTRASONDO, DEVOLVIENDO EL VALOR DE LA DISTANCIA QUE MIDE PARA UTILIZARLA
*/
long ultrasonidos(int pin_trig_sensor,int pin_echo_sensor) {
  long distancia;
  unsigned long tiempo;
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(pin_trig_sensor, LOW);
  delayMicroseconds(2);

  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(pin_trig_sensor, HIGH);
  delayMicroseconds(10);

  
  digitalWrite(pin_trig_sensor, LOW); // Comenzamos poniendo el pin Trigger en estado bajo

  tiempo = pulseIn(pin_echo_sensor, HIGH);   //Calculo del tiempo que mide el sensor

  //Serial.print(tiempo);
  distancia = tiempo * 0.000001 * 34000 / 2.0; //Calculo de la distancia partiendo del tiempo
  Serial.print(distancia);
  Serial.print("cm");
  Serial.println();
  return distancia;
}
/*
*A PARTIR DE AQUI TODO SON MANIOBRAS DE MOVIMIENTO
*/
void girarDerecha() {
  Serial.println("Girando a la derecha...");
  servoDerecho.write(120);
  servoIzquierdo.write(120);
  delay(50);
}
void girarIzquierda() {
  Serial.println("Girando a la izquierda...");
  servoDerecho.write(0);
  servoIzquierdo.write(0);
  delay(50);
}
void adelante() {
  Serial.println("ATACANDO adelante");
  servoDerecho.write(0);
  servoIzquierdo.write(180);
  delay(100);
}

void atras() {
  Serial.println("Moviendo atras...");
  servoDerecho.write(180);
  servoIzquierdo.write(0);
  delay(50);
}

void parado() {
  Serial.println("Parao");
  servoDerecho.write(90);
  servoIzquierdo.write(90);
  delay(50);
}

void evasion() {
  parado();
  delay(100);
  Serial.println("Evasion");
  atras();
  delay(1000);
}
