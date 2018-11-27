#define BOTTOM 255-187
uint16_t volatile setup_time=0; 
uint8_t update=0,move=0,turn=0;
uint16_t  speed, speed_ref, direction, direction_ref;
float  pwm1=0.5, pwm2=0.5, control, kp, ki;

ISR(TIMER2_OVF_vect){
    if(setup_time<1666){   //5 segundos. esperar para permitir o ESC reconhecer o stop e calibrar
      setup_time++;
      TCNT2 = BOTTOM;
      OCR2A = (255-187)+(255-(255-187))*0.5;
      OCR2B = (255-187)+(255-(255-187))*0.5;
    }
    else if(setup_time>=1666){
       TCNT2 = BOTTOM;
       if (pwm1>0.633) pwm1=0.633; //limite superior
       if (pwm2>0.633) pwm2=0.633; //limite superior
       if (pwm1<0.367) pwm1=0.367; //limite inferior
       if (pwm2<0.367) pwm2=0.367; //limite inferior
       OCR2A = (255-187)+(255-(255-187))*pwm1;
       OCR2B = (255-187)+(255-(255-187))*pwm2;
    }
      
  }
  
///////////////////////////////////////////////////////////////////////////////////////////////////////

void init_timer() {      //para um duty cycle de 0.5 a contagem é 1500us 
  DDRB |= (1 << DDB3); //definir PB3 como output (PB3) é a porta correspondente à saída de comparação OC2A  
  TCCR2B = 0; // Stop TC2 
  CLKPR = 0x80; // Prepare bits 
  CLKPR = 0x00; // CLK prescaler at 1 
  TCCR2A |= (1 << WGM21) | (1 << WGM20); // set fast PWM Mode 
  TCCR2A |= (1 << COM2A1); // set none-inverting mode
  TCCR2A |= (1 << COM2B1); // set none-inverting mode
  TIMSK2=0x01; //interrupçao tc2 por overflow 
  TCNT2=BOTTOM; // definir bottom 
  OCR2A = (255-187)+(255-(255-187))*0.5; // set PWM1 for 50% duty cycle    //pino 11
  OCR2B = (255-187)+(255-(255-187))*0.5; // set PWM2 for 50% duty cycle    //pino 3
  TCCR2B=0x06; // Start TC2 with prescaler at 256 -> periodo = (256-BOTTOM)*(1/16000000)*256 = 3000 us = 1500*2 us =             ... 20000 us
  sei(); 
}

void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT); 
  init_timer();
}


int speed_control(int speed_error) {
    //altera PWM entre 0.5 e 0.633 conforme speed_error. Usar controlador PI
    control=0.00133*speed_error;//+ki*integral(speed_error) //como se faz o integral?? (speed_error_atual+speed_error_antigo)*tempo ?
    if(((pwm1+control)<0.633)&&((pwm2+control)<0.633)){
      pwm1=pwm1+control;
      pwm2=pwm2+control;
    }
    else if(((pwm1+control)>0.633)||((pwm2+control)>0.633)){
      pwm1=0.633;
      pwm2=0.633;
    }
    return 1;
}

void loop() {
 if (Serial.available() > 0) {
    int number = Serial.parseInt();
    if((number>=-99)&&(number<=99)){
    speed_control(number);
    Serial.print("I received: ");
    Serial.println(number, DEC);
    Serial.print("pwm1 is: ");
    Serial.println(pwm1, DEC);
    Serial.print("pwm2 is: ");
    Serial.println(pwm2, DEC);
    } 
  }
} 
