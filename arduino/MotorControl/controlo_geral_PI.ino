#define BOTTOM 255-187
uint16_t volatile setup_time=0;
uint8_t id=0; 
float  pwm1=0.5, pwm2=0.5;
uint8_t speed_error_vector[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t direction_error_vector[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int speed_error_sum, direction_error_sum;
float speed_error_integral, direction_error_integral;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// TIMER2_ISR

ISR(TIMER2_OVF_vect){    //interrupt a cada 3 ms
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
       OCR2B = (255-187)+(255-(255-187))*pwm2;;    //os thrusters rodam em sentidos opostos
    }

  speed_error_integral = speed_error_sum*0.003*10;  //soma*deltat (3 ms * 10)
  direction_error_integral = direction_error_sum*0.003*10;  //soma*deltat (3 ms * 10)
  
  }
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// INIT_TIMER

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SETUP

void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT); 
  init_timer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SPEED_CONTROL

int speed_control(int speed_error) {
  speed_error_vector[10] = speed_error;
  speed_error_sum=0;  
  for(int8_t i=1; i<=10; i++){
  speed_error_sum = speed_error_sum + speed_error_vector[i];   //o speed_error_integral é calculado no interrupt do timer para se saber o tempo exato que passou
  if (i>1) speed_error_vector[i-1]=speed_error_vector[i];    
  }
  speed_error_vector[10]=0;
  
    float control=0.00133*speed_error+1*speed_error_integral;  //afinar ganhos
    if( pwm1+control<0.633 && pwm2+control<0.633 && pwm1+control>0.367 && pwm2+control>0.367){
      pwm1=pwm1+control;
      pwm2=pwm2+control;
    }
    else if(((pwm1+control)>0.633)||((pwm2+control)>0.633)){
      pwm1=0.633;
      pwm2=0.633;
    }
    else if(((pwm1+control)<0.367)||((pwm2+control)<0.367)){
      pwm1=0.367;
      pwm2=0.367;
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DIRECTION_CONTROL

int direction_control(int direction_error) {
  direction_error_vector[10] = direction_error;
  direction_error_sum=0;  
  for(int8_t i=1; i<=10; i++){
  direction_error_sum = direction_error_sum + direction_error_vector[i];   //o speed_error_integral é calculado no interrupt do timer para se saber o tempo exato que passou
  if (i>1) direction_error_vector[i-1] = direction_error_vector[i];    
  }
  direction_error_vector[10]=0;
    
    float control=0.00133*direction_error+1*direction_error_integral; //afinar ganhos 
    if (direction_error==0){
      if((pwm1>0.5 && pwm2<0.5)||(pwm1<0.5 && pwm2>0.5)){  //se thrusters estiverem em sentidos contrarios um do outro
        pwm1=0.5;
        pwm2=0.5;
      }
      else if(pwm1<pwm2 && pwm1>0.5 && pwm2>0.5){         //se a andar para a frente, fica com a velocidade do thruster mais lento
        pwm2=pwm1;
      }
      else if (pwm1>pwm2 && pwm1>0.5 && pwm2>0.5){      //se a andar para a frente, fica com a velocidade do thruster mais lento
        pwm1=pwm2;
      }
      else if (pwm1<pwm2 && pwm1<0.5 && pwm2<0.5){      //se a andar para tras, fica com a velocidade do thruster mais lento
        pwm1=pwm2;
      }
      else if (pwm1>pwm2 && pwm1<0.5 && pwm2<0.5){        //se a andar para tras, fica com a velocidade do thruster mais lento
        pwm2=pwm1;
      }
    }
    else if (direction_error>0){                        //esquerda
      if (pwm1==0.5 && pwm2==0.5){                      //se estiver parado, thrusters rodam em sentidos opostos para rodar sobre o seu eixo
        pwm1=pwm1+control;
        pwm2=pwm2-control;
      }
      else if(pwm2+control<0.633 && pwm1+control>0.367 && pwm1>0.5 && pwm2>0.5){    //se a andar para a frente, aumenta o thruster da direita no sentido positivo
        pwm1=pwm2+control;
      }
      else if(pwm2+control<0.633 && pwm1+control>0.367 && pwm1<0.5 && pwm2<0.5){   //se a andar para a tras, aumenta o thruster da direita no sentido negativo
        pwm1=pwm2-control;
      }
      else if(pwm2+control>0.633){  //maximo
        pwm1=0.633;
      }
      else if(pwm2+control<0.367){  //maximo
        pwm1=0.367;
      }
    }
    else if (direction_error<0){                                //direita
      if (pwm1==0.5 && pwm2==0.5){                              //se estiver parado, thrusters rodam em sentidos opostos para rodar sobre o seu eixo
        pwm1=pwm1-control*(-1);
        pwm2=pwm2+control*(-1);
      }
      else if(pwm1+control*(-1)<0.633 && pwm1+control*(-1)>0.367 && pwm1>0.5 && pwm2>0.5){    //se a andar para a frente, aumenta o thruster da esquerda no sentido positivo
        pwm2=pwm1+control*(-1);
      }
      else if(pwm1+control*(-1)<0.633 && pwm1+control*(-1)>0.367 && pwm1<0.5 && pwm2<0.5){    //se a andar para tras, aumenta o thruster da esquerda no sentido negativo
        pwm2=pwm1-control*(-1);
      }
      else if(pwm1+control*(-1)>0.633){  //maximo
        pwm2=0.633;
      }
      else if(pwm1+control*(-1)<0.367){  //maximo
        pwm2=0.367;
      }
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// RECEIVE_MESSAGE_ID

void receive_message_id(){
   if (Serial.available() > 0) {
    if(id==0){
    id = Serial.parseInt();;
      if (id!=0){
        Serial.print("message id: ");
        Serial.println(id);
        //delay(1000);               //delay de 1 segundo para introduçao manual do valor. quando ligar a rasp, pode-se tirar, porque ja tem um timeout intrinseco
        receive_value();
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// RECEIVE_VALUE

void receive_value(){
    if (Serial.available() > 0) {
      if(id==1){
        int number = Serial.parseInt();
        if((number>=-99)&&(number<=99)){
          Serial.print("I received speed_error: ");
          Serial.println(number, DEC);
          speed_control(number);
          id=0;
          Serial.print("pwm1 is: ");
          Serial.println(pwm1, DEC);
          Serial.print("pwm2 is: ");
          Serial.println(pwm2, DEC);
        }
      }
    
    else if(id==2){
      int number = Serial.parseInt();
      if((number>=-99)&&(number<=99)){
        Serial.print("I received direction_error: ");
        Serial.println(number, DEC);
        direction_control(number);
        id=0;
        Serial.print("pwm1 is: ");
        Serial.println(pwm1, DEC);
        Serial.print("pwm2 is: ");
        Serial.println(pwm2, DEC);        
      }
    }
   else if(id==3){
    //enviar temperatura
    //get_temperature();
    //Serial.print("temperature");
    //Serial.println(temperature, DEC);
    id=0;
   }
   else id=0;
  } 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// LOOP

void loop() {
    receive_message_id();     
}
