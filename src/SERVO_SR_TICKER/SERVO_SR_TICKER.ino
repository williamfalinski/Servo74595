/*
 *  Software para controle de servos SG90 com  
 *  conversor Serial/Paralelo SN74HC595  
 *  Testado em NodeMCU 1.0 ESP12-E 
 *  Pinos do 74595 -> DATA - D_CLK - LATCH 
 *  Pinos utilizados do ESP8266 -> 12 - 13 - 14
 *
 *   Universidade Estadual de Ponta Grossa 2018
 *   Engenharia de Computação
 *   Sistemas Embarcados
 *
 *   Autores:   William B. Falinski
 *              Wendel Luiz
*/

#include <Ticker.h>
/*Valores especificos para o SG90*/
#define MAX_DUTY_CYCLE 2450//us
#define MIN_DUTY_CYCLE 550 //us
#define PWM_PERIOD     20  //ms
/*******************************************/
#define DATA      12//Pino Dados SER
#define D_CLK     13//Pino Clock de comunicacao SRCLK
#define LATCH     14//Pino Latch RCLK
#define N_SERVOS  16 //Quantidade de Servos 

Ticker   t_pwm;   //Ticker para delay de 20000us

uint16_t DUTY_CYCLES[N_SERVOS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Periodo em HIGH de cada servo 
/*TODO: UTILIZAR BYTE(s)*/
uint8_t  DATA_SERVOS[N_SERVOS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Vetor de Binários para enviar ao 74595
uint64_t PWM_Start;
uint64_t PWM_Now;

/*INICIALIZA SERVOS EM 0º*/
/*Especificar numero de servos para evitar calcular servos inexistentes*/
void initialize_servos(){
  for(int i = 0; i<N_SERVOS; i++){
    DUTY_CYCLES[i] = MIN_DUTY_CYCLE;
  }
}

/*Handler da interrupção do ticker (ISR)*/
void servoISR(){
  PWM_Start = micros();
  PWM_Now = micros() - PWM_Start;
  
  /*Enquanto nao completar um pulso*/
  while(PWM_Now <= MAX_DUTY_CYCLE){
    setBits(PWM_Now);
    loadData_CI(); 
    PWM_Now = (micros() - PWM_Start); 
  }
  /*Zera o resto do periodo*/
  setBits(20000);
  loadData_CI(); 
}

/*Vetor de Bits para enviar ao 74595*/
void setBits(uint64_t t_now){
  for(int i = 0; i < N_SERVOS; i++){
    if(t_now < DUTY_CYCLES[i]){
       DATA_SERVOS[i] = HIGH;
    }
    else{
       DATA_SERVOS[i] = LOW;
    }
  }
}
/*GRAVA INFORMAÇÕES NO 74595*/
void loadData_CI(){
  /*GRAVA DO ULTIMO PARA O PRIMEIRO BIT*/
  /*CLOCK DO LATCH*/ 
  digitalWrite(LATCH, LOW);
  for(int i=N_SERVOS-1; i>=0; i--){
    /*CLOCK DOS REGISTRADORES PRIMARIOS*/
    digitalWrite(D_CLK, LOW);
    digitalWrite(DATA, DATA_SERVOS[i]);
    digitalWrite(D_CLK, HIGH);
    /*FIM CLOCK DOS REGISTRADORES PRIMARIOS*/
  }
  digitalWrite(LATCH, HIGH); 
  /*FIM CLOCK DO LATCH*/
}

int pass = 0;
int inc = -36;

void setup() {
  pinMode(DATA, OUTPUT);
  pinMode(D_CLK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  t_pwm.attach_ms(PWM_PERIOD, servoISR); //Ticker com periodo PWM em milisegundos e função ISR 
}

void loop() {
  if(pass == 180 || pass == 0) 
    inc *= -1;
  for(int i = 0; i < N_SERVOS; i++){
    DUTY_CYCLES[i] = map(pass, 0, 180, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    delay(100);
  }  
  pass += inc; 
  
}
