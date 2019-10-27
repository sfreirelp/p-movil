/*

 Papelera Móvil V 1.0 - (C) 2019 Santiago Freire. Todos los derechos reservados.
 
 Conexiones.
 Digitales.
 
 TRACKER IZQ: 5
 TRACKER DER: 6
 ULTRASONIC ECHO: 8
 ULTRASONIC TRIG: 9
 
 BLUETOOTH RX: 10
 BLUETOOTH TX: 11
 BLUETOOTH KEY: 12
 
 SERVO: 7
 STEP:
 IN1: 24
 IN2: 26
 IN3: 28
 IN4: 30
 DRIVER L298N:
 ENA: 34
 IN1: 36
 IN2: 38
 IN3: 40
 IN4: 42
 ENB: 44
 
 */

//DEFINICIONES

#include <SoftwareSerial.h>
#include <Servo.h>
#include <Stepper.h>

#define TrackIzq 5
#define TrackDer 6
#define ServoPin 7
#define trigPin 9
#define echoPin 8
#define BT_RX 10
#define BT_TX 11
#define BT_KEY 12
#define ST_IN1 24
#define ST_IN2 26
#define ST_IN3 28
#define ST_IN4 30
#define DRV_ENA 34
#define DRV_IN1 36
#define DRV_IN2 38
#define DRV_IN3 40
#define DRV_IN4 42
#define DRV_ENB 44

#define STEPS 32 // STEPS para el Stepper

int command; //Para BT
long duration; //Para ultrasonic
int distance; //Para ultrasonic
unsigned long t_inicial; //Tiempo cuando arranca el seguidor para adelante (le envían 1)
unsigned long t_final; //Tiempo cuando para (le envían 0)
unsigned long millis_inicial; // Millis inicial para seguidor inverso (le envían -1)
int unavezAdelante = 0; //Para repetir una acción 1 vez (en ADELANTE)
int unavezParar = 0; //Para repetir una acción 1 vez (en PARAR)
int unavezFinal = 0; //Para repetir una acción 1 vez (en SeguidorInverso)
int unavezAbrir = 0;
int unavezCerrar = 0;
unsigned long TiempoTranscurrido; //Variable donde se calcula y guarda el tiempo transcurrido desde que se comenzó a mover el robot hasta que paró
int motor_speed_IZQ = 255; //IZQ: DRV_IN1 y DRV_IN2
int motor_speed_DER = 255; //DER: DRV_IN3 y DRV_IN4
int vuelta_step = 2048; //Una vuelta entera del step equivale a 2048 pasos
int completo_step = 6000; //Cantidad de pasos necesarios para abrir / cerrar la tapa. ESTE VALOR DEBE CALIBRARSE EN PRESENCIA DEL HARDWARE
int servo_grados = 30; //Grados de giro para que el servo apunte a la derecha o izquierda (CALIBRAR)
int servo_cero = 90; //Grados de giro para que el servo vuelva a su posición original (cero) (CALIBRAR)


Servo myservo; //Creo un servo llamado 'myservo'
Stepper stepper (STEPS, ST_IN1, ST_IN2, ST_IN3, ST_IN4); //Creo un stepper llamado 'stepper', conectado mediante un driver a los pines de las definiciones.
SoftwareSerial MiBT (10, 11); // RX | TX (para módulo Bluetooth HC-05)

//VOIDS
//Para llamar void funcion() { }, usar funcion();

void ADELANTE () {
  //Programa para seguir adelante (dejar servo de rueda loca en 0 y mandar el driver hacia adelante)

  digitalWrite(DRV_IN1, LOW);
  digitalWrite(DRV_IN2, HIGH);
  digitalWrite(DRV_IN3, HIGH);
  digitalWrite(DRV_IN4, LOW);
  analogWrite(DRV_ENA, motor_speed_IZQ);
  analogWrite(DRV_ENB, motor_speed_DER);

  myservo.write(servo_cero); //Dejo la rueda loca en 0
}

void ATRAS () {
  //Programa para ir hacia atrás (dejo la rueda loca en 0 y mando al driver que rote los motores hacia atrás)

  digitalWrite(DRV_IN1, HIGH);
  digitalWrite(DRV_IN2, LOW);
  digitalWrite(DRV_IN3, LOW);
  digitalWrite(DRV_IN4, HIGH);
  analogWrite(DRV_ENA, 255);
  analogWrite(DRV_ENB, 255);

  myservo.write(servo_cero); //Dejo la rueda loca en 0
}

void IZQUIERDA () {
  //Programa para girar el robot a la izquierda (rueda loca a la izq)

  // myservo.write((servo_grados * (-1))); //Giro rueda loca a la izquierda

  digitalWrite(DRV_IN1, LOW);
  digitalWrite(DRV_IN2, LOW);
  digitalWrite(DRV_IN3, HIGH);
  digitalWrite(DRV_IN4, LOW);
  //analogWrite(DRV_ENA, 255);
  analogWrite(DRV_ENB, 255);


}

void DERECHA () {
  //Programa para girar el robot a la derecha (rueda loca a la derecha)

  // myservo.write(servo_grados); //Giro la rueda loca a la derecha

  digitalWrite(DRV_IN1, LOW);
  digitalWrite(DRV_IN2, HIGH);
  digitalWrite(DRV_IN3, LOW);
  digitalWrite(DRV_IN4, LOW);
  analogWrite(DRV_ENA, 255);
  //analogWrite(DRV_ENB, 255);

}

void DERECHA_ATRAS () {
  //Programa para ir hacia atrás (dejo la rueda loca en 0 y mando al driver que rote los motores hacia atrás)

  digitalWrite(DRV_IN1, LOW);
  digitalWrite(DRV_IN2, LOW);
  digitalWrite(DRV_IN3, LOW);
  digitalWrite(DRV_IN4, HIGH);
  analogWrite(DRV_ENA, 255);
  analogWrite(DRV_ENB, 255);

  //  myservo.write(servo_cero); //Dejo la rueda loca en 0
}

void IZQUIERDA_ATRAS () {
  //Programa para ir hacia atrás (dejo la rueda loca en 0 y mando al driver que rote los motores hacia atrás)

  digitalWrite(DRV_IN1, HIGH);
  digitalWrite(DRV_IN2, LOW);
  digitalWrite(DRV_IN3, LOW);
  digitalWrite(DRV_IN4, LOW);
  analogWrite(DRV_ENA, 255);
  analogWrite(DRV_ENB, 255);

  //  myservo.write(servo_cero); //Dejo la rueda loca en 0
}

void PARAR () {
  //Programa para detener todos los motores (y dejar la rueda loca en 0)

  digitalWrite(DRV_IN1, LOW);
  digitalWrite(DRV_IN2, LOW);
  digitalWrite(DRV_IN3, LOW);
  digitalWrite(DRV_IN4, LOW);

  myservo.write(servo_cero); //Dejo la rueda loca en 0

}

void ABRIR() {

  unavezCerrar = 0;

  if (unavezAbrir == 0) {
    stepper.step(completo_step);
    unavezAbrir++;

  }

}

void CERRAR() {

  unavezAbrir = 0;

  if (unavezCerrar == 0) {
    stepper.step((completo_step * (-1)));
    unavezCerrar++;

  }


}

void case0 () {

  //Me detengo
  //Guardo el tiempo final (tiempo transcurrido hasta que paré) en la variable 'unavezParar'
  if (unavezParar == 0) {

    t_final = millis();
    Serial.println("Tiempo final: ");
    Serial.print(t_final);
    unavezParar++;
  }

  PARAR();
  // CERRAR();

}

void case1 () {
  //Sigo la línea y abro la tapa, mientras la distancia sea mayor a 10

  DISTANCIA(); //Llamo al medidor de distancia con el ultrasónico

  if (distance >= 10) {  //NOTA: LUEGO FIJARSE A CUÁNTO EQUIVALE LA DISTANCIA DE 10 CM EN VALOR SIN PROCESAR - PROCESÁNDOLO DEMORA MUCHO EN CALCULARLO Y DA ERRORES (COMO UN VALOR REPENTINAMENTE ALTO)
    ABRIR(); //Abrir tapa

    //Para obtener el valor de millis una sola vez, la primera vez que se corre, me fijo si la variable 'unavezAdelante' creada al principio del programa vale 0.
    //Si ella vale 0, guardo el tiempo que corrió el arduino en la variable t_inicial, y le sumo 1 a 'unavezAdelante' - de esta forma, me aseguro de que esto se corra una sola vez y no en loop, porque si no daría resultados incorrectos
    //La segunda vez la condición no se va a cumplir, y la variable va a quedar intacta.

    if (unavezAdelante == 0) {
      t_inicial = abs(millis());
      unavezAdelante++;
      Serial.println("Tiempo inicial: ");
      Serial.print(t_inicial);

    }
    SeguirLinea(); //Llamo al seguidor de linea
  }

  else {
    PARAR();
    MiBT.println("Objeto detectado -- parando");
    delay(150); //Para dar estabilidad
  }
}

void DISTANCIA () {

  // Resetea el TrigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // TrigPin en HIGH por 10 microsegundos
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Lee el EchoPin y devuelve el tiempo que demora en viajar
  duration = pulseIn(echoPin, HIGH);

  // Calcula la distancia
  distance = duration * 0.034 / 2; //NOTA: LUEGO FIJARSE A CUÁNTO EQUIVALE LA DISTANCIA DE 10 CM EN VALOR SIN PROCESAR - PROCESÁNDOLO DEMORA MUCHO EN CALCULARLO Y DA ERRORES (COMO UN VALOR REPENTINAMENTE ALTO)

}

void FIN () {
  PARAR(); // FIN DEL PROGRAMA

  //Reseteo todas las variables para poder volver a repetir estas acciones sin problemas
  duration = 0;
  distance = 0;
  t_inicial = 0;
  t_final = 0;
  millis_inicial = 0;
  TiempoTranscurrido = 0;
  unavezAdelante = 0;
  unavezParar = 0;
  unavezFinal = 0;
  Serial.println("FIN");

  command = 9; //Finalmente, reseteo 'command', dándole un valor diferente a cualquier opción del switch case para que no se repitan estas acciones indefinidamente.

  //FIN DEL PROGRAMA

}

void SeguirLinea () {

  do  {
    //DISTANCIA();
    //Serial.println(distance);
    command = MiBT.read(); //Guardo en la variable command todo lo que recibo por BT

    //Lógica clásica de un seguidor de línea
    if (digitalRead(TrackIzq) == 0) {
      if (digitalRead(TrackDer) == 0) {
        ADELANTE();
      }
      else {
        DERECHA();
      }
    }
    else {
      IZQUIERDA();
    }
  }
  while (command != '0');

  //    while (duration >= '600' ^ command != '0');

  case0();
}
//No es necesario hacer un seguidor inverso. Es lo mismo que el normal pero yendo hacia atrás.
//De cualquier forma, habría que verificarlo. --> HAY QUE HACER UN SEGUIDOR INVERSO.

void SeguidorInverso () {
  do {
    Serial.println((millis() - millis_inicial));
    Serial.print(millis());

    if (digitalRead(TrackIzq) == 0) {
      if (digitalRead(TrackDer) == 0) {
        ATRAS();
      }
      else {
        DERECHA_ATRAS();
      }
    }
    else {
      IZQUIERDA_ATRAS();
    }
  } 
  while ((millis() - millis_inicial) <= TiempoTranscurrido);
  FIN();

}

void setup() {

  MiBT.begin(9600); //BT
  Serial.begin(9600); //Monitor serial para debugging
  myservo.attach(ServoPin); //Servo en pin 7
  pinMode(trigPin, OUTPUT); //Ultrasonic
  pinMode(echoPin, INPUT); //Ultrasonic
  pinMode(TrackIzq, INPUT); //Tracker izquierda
  pinMode(TrackDer, INPUT); //Tracker derecha

  //DRIVER L298N
  pinMode(DRV_ENA, OUTPUT);
  pinMode(DRV_IN1, OUTPUT);
  pinMode(DRV_IN2, OUTPUT);
  pinMode(DRV_IN3, OUTPUT);
  pinMode(DRV_IN4, OUTPUT);
  pinMode(DRV_ENB, OUTPUT);

  //STEPPER
  stepper.setSpeed(500); //Seteo la velocidad en 200 (máximo permitido)

}

void loop() {

  if (MiBT.available()) {
    command = MiBT.read(); //Guardo en la variable command todo lo que recibo por BT

    //Si me envían '1', abro la tapa y sigo la línea. Si me envían '0', paro. Si me envían '-1', cierro la tapa y hago el seguidor inverso por el mismo tiempo que hice el seguidor normal para terminar en la misma posición donde empecé.
    //La idea es que el orden de envío de datos sea 1 --> 0 --> -1. Si se envía en otro orden, puede causar ciertos problemas.

    switch (command) { //Switch case con la variable 'command'
    case '1':
      do { 
        case1();
      } 
      while (command != '0');
      break;

    case '0':
      //Me detengo
      //Guardo el tiempo final (tiempo transcurrido hasta que paré) en la variable 'unavezParar'
      if (unavezParar == 0) {
        t_final = abs(millis());
        Serial.print(t_final);
        unavezParar++;
      }

      PARAR();

      break;

    case '2':
      do {   //Cierro la tapa y vuelvo a mi posición original con el seguidor inverso, haciéndolo durante el tiempo que me tomó llegar hasta allí
        //El seguidor inverso es como el seguidor normal, solo que se hace mientras el robot va hacia atrás -- VERIFICAR CON HARDWARE LISTO - PUEDE SER QUE HAYA QUE INTERCAMBIAR SENSORES EN CONDICIONALES (TrackIzq por TrackDer y viceversa)

        //NOTA: EN ESTA PARTE NO SE NECESITA EL ULTRASONICO PORQUE SE SUPONE QUE SI EL ROBOT YA FUE HACIA ADELANTE SIN OBSTÁCULOS, A LA VUELTA TEÓRICAMENTE TAMPOCO DEBERÍA HABERLOS - NO ES LÓGICO QUE HAYA OBSTÁCULOS NUEVOS EN LA PISTA EN UN PROYECTO DE ESTE TIPO

        TiempoTranscurrido = abs(t_final - t_inicial); //Calculo delta T (tiempo transcurrido en total)
        Serial.println("Tiempo transcurrido: ");
        Serial.print(TiempoTranscurrido);

        CERRAR(); //Cerrar tapa

        // ATRAS();

        if (unavezFinal == 0) {
          millis_inicial = abs(millis()); //millis_inicial es solo para esta parte. No confundirse con otra variable anterior (t_inicial)
          Serial.println("Millis segunda parte: ");
          Serial.println(millis_inicial);
          //millis_inicial es la cantidad de millis transcurridos cuando el robot comienza a hacer el seguidor inverso. Se usa para calcular el tiempo transcurrido.
          unavezFinal++; //Para no repetirlo dos veces
        }


        //millis() > millis_inicial. Por tanto, su resta da positiva. Esta resta se va a ir incrementando de a poco, hasta que en un momento va a igualar a TiempoTranscurrido.
        //Es aquí cuando va a haber seguido la línea por el mismo tiempo que lo hizo en una primera instancia; y ahí es donde para, para luego volver a estar pronto para salir si lo llaman.
        //Mayor o igual por las dudas que se pase. No creo que llegue nunca a ser exacto
        //(millis - millis_inicial) da el tiempo transcurrido desde que está haciendo el seguidor inverso. Una vez que lo haya hecho por el mismo tiempo (o más, por las dudas) que el seguidor 'normal', para, y termina el programa.
        //Finalmente, resetea todas las variables para que el programa pueda volver a correr normalmente desde el principio.

        if ( (millis() - millis_inicial) >= TiempoTranscurrido) {
          PARAR(); // FIN DEL PROGRAMA

          //Reseteo todas las variables para poder volver a repetir estas acciones sin problemas
          duration = 0;
          distance = 0;
          t_inicial = 0;
          t_final = 0;
          millis_inicial = 0;
          TiempoTranscurrido = 0;
          unavezAdelante = 0;
          unavezParar = 0;
          unavezFinal = 0;
          Serial.println("FIN");

          command = 9; //Finalmente, reseteo 'command', dándole un valor diferente a cualquier opción del switch case para que no se repitan estas acciones indefinidamente.

          //FIN DEL PROGRAMA


        }

        else {
          SeguidorInverso(); //Si no me pasé de tiempo, sigo la línea.
        }

        break;

      } 
      while (command != '0');
    }


  }

}

