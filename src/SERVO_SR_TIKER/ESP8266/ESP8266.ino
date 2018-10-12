/*Software para controle de servos SG90 com shift register 74595 
 *testado no ESP8266 utilizando o Ticker
 *Pinos do 74595 -> DATA - D_CLK - LATCH 
 *Pinos utilizados do ESP8266 -> 12 - 13 - 14 - A0(Potenciometro)
*/
#include <Ticker.h>
/*Valores especificos para o SG90 XING LING*/
#define MAX_DUTY_CYCLE 2450//us
#define MIN_DUTY_CYCLE 550 //us
#define PWM_PERIOD     20  //ms
/*******************************************/
#define DATA      12//Pino Dados SER
#define D_CLK     13//Pino Clock de comunicacao SRCLK
#define LATCH     14//Pino Latch RCLK
#define POT       A0//Pino Potenciômetro
#define N_SERVOS  8 //Quantidade de Servos 

Ticker   t_pwm;   //Ticker para delay de 20000us
uint16_t valPot;  //Potenciometro
uint16_t DUTY_CYCLES[8] = {0,0,0,0,0,0,0,0}; //Periodo em HIGH de cada servo 
/*TODO: UTILIZAR BYTE(s)*/
uint8_t  DATA_SERVOS[8] = {0,0,0,0,0,0,0,0}; //Vetor de Binários para enviar ao 74595
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
void servoPulse(){
  PWM_Start = micros();
  PWM_Now = micros() - PWM_Start;
  
  /*Enquanto nao completar um pulso*/
  while(PWM_Now <= MAX_DUTY_CYCLE){
    setBits(PWM_Now);
    loadData_74595(); 
    PWM_Now = (micros() - PWM_Start); 
  }
  /*Zera o resto do periodo*/
  setBits(20000);
  loadData_74595(); 
}

/*Vetor de Bits para enviar ao 74595*/
void setBits(uint64_t pwm_now){
  for(int i = 0; i < N_SERVOS; i++){
    if(pwm_now < DUTY_CYCLES[i]){
       DATA_SERVOS[i] = HIGH;
    }
    else{
       DATA_SERVOS[i] = LOW;
    }
  }
}
/*GRAVA INFORMAÇÕES NO 74595*/
void loadData_74595(){
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

void setup() {
  pinMode(POT, INPUT);
  pinMode(DATA, OUTPUT);
  pinMode(D_CLK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  t_pwm.attach_ms(20, servoPulse); //Ticker com periodo PWM em milisegundos e função ISR 
}

void loop() {
  DUTY_CYCLES[1] = map(analogRead(POT), 0, 1023, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE); 
}
