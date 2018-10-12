#include <Ticker.h>

/*Valores especificos para o SG90 XING LING*/
/*
#define MAX_DUTY_CYCLE 2450  //us
#define MIN_DUTY_CYCLE 550   //us
#define PWM_PERIOD     20 //ms
#define N_SERVOS  8 //Quantidade de Servos 
#define SER       12//Pino Dados
#define SRCLK     13//Pino Clock de comunicacao
#define RCLK      14//Pino Latch
#define POT       AD0//Potenciometro
*/

#define SERVO 4
#define POT   A0

Ticker t_pwm; //Ticker para delay de 20000us
uint16_t valPot;

void servoPulse(){
  digitalWrite(SERVO, HIGH);
  delayMicroseconds(valPot);
  digitalWrite(SERVO, LOW);
}


void setup() {
  Serial.begin(115200);
  pinMode(POT, INPUT);
  pinMode(SERVO, OUTPUT);
  t_pwm.attach_ms(20, servoPulse); //Ticker com periodo PWM em milisegundos e função ISR 
}

void loop() {
  valPot = map(analogRead(POT), 0, 1023, 550, 2450); 
  //Serial.println(valPot);
}
