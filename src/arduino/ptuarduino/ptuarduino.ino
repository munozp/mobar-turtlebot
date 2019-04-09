//#include "arduino.h"
#include <Servo.h>

#define TMILLIS_OVERFLOW 4294967295


char lectura[50];
char respuesta[50];
char variable[10];
char *frase;
int angulo = 90;
bool flag_lec = false;
int anguloYaw = 90;
int anguloPitch = 90;
int k=0;

Servo yaw; //horizontal
Servo pitch; //elevacion

void setup(){
  Serial.begin(9600); //57600
  SerialBufferClean ();
  yaw.attach(8);
  pitch.attach(9);
}
void loop(){
    
    if(Serial.available()>0)
    {
      lectura[k] = Serial.read();
      if(lectura[k] == ';'){
        flag_lec = true;
      }
      k++;
    }
     
  if(flag_lec){
    strcpy(respuesta, lectura);
    frase = strtok(lectura, ":");
    while (frase != NULL) 
    {
          sscanf(frase, "%s %d", variable, &angulo);
          angulo = constrain(angulo, 0, 180);   
          if(!strcmp("Yaw", variable)) anguloYaw = angulo;
          if(!strcmp("Pitch", variable)) anguloPitch = angulo;
          frase = strtok(NULL, ";");
    }
    flag_lec = false;
    k = 0;
    Serial.write(respuesta);
  }
  
  yaw.write(anguloYaw);
  pitch.write(anguloPitch);
}

void SerialBufferClean (){	//limpiar buffer, en las ultimas versiones cambiaron Serial.flush() y ya no hace lo mismo
	while (Serial.available() > 0) Serial.read(); 
}
