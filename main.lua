#include <Servo.h>

// Crear un objeto Servo para controlar el servomotor
Servo myservo;

// Definir el pin del botón y el pin del servomotor
const int buttonPin = 2;
const int servoPin = 9;

// Variable para almacenar el estado del botón
int buttonState = 0;

// Posición inicial del servomotor
int servoPos = 0;

void setup() {
    // Iniciar el servo en el pin especificado
    myservo.attach(servoPin);
    
    // Configurar el pin del botón como entrada con resistencia pull-up interna
    pinMode(buttonPin, INPUT_PULLUP);
    
    // Configurar la posición inicial del servomotor
    myservo.write(servoPos);
}

void loop() {
    // Leer el estado del botón
    buttonState = digitalRead(buttonPin);
    
    // Si el botón está presionado (estado bajo porque usamos pull-up)
    if (buttonState == LOW) {
        // Mover el servomotor a 180 grados
        myservo.write(180);
        
        // Esperar un segundo
        delay(1000);
        
        // Volver el servomotor a 0 grados
        myservo.write(0);
    }
}