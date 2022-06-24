void process_radio() {
  //process the radio stuff. 
  if (pwm_CH1_value > 500 && pwm_CH1_value < 2500){
    CH1 = pwm_CH1_value; //current Channel 1 Value
  }
  if (pwm_CH2_value > 500 && pwm_CH2_value < 2500){
    CH2 = pwm_CH2_value; //current Channel 2 Value
  }
  //autoset the max and min CH1 and CH2 seen
  if(CH1 < CH1_min) {
    CH1_min = CH1;
  }
  if(CH1 > CH1_max) {
    CH1_max = CH1;
  }
  if(CH2 < CH2_min) {
    CH2_min = CH2;
  }
  if(CH2 > CH2_max) {
    CH2_max = CH2;
  }
  if (print_serial) {
  Serial.print("CH1 Min: ");
  Serial.print(CH1_min);
  Serial.print("\t");
  Serial.print("CH1: ");
  Serial.print(CH1);
  Serial.print("\t");
  Serial.print("CH1_Max: ");
  Serial.print(CH1_max);
  Serial.print("\t");
  Serial.print("CH2_Min: ");
  Serial.print(CH2_min);
  Serial.print("\t");
  Serial.print("CH2: ");
  Serial.print(CH2);
  Serial.print("\t");
  Serial.print("CH2_Max: ");
  Serial.println(CH2_max);
  }
}


// Below are the INT0 and INT1 functions that read the RC PWM inputs on Pins 2 and 3. 
void ch1_rising() {
  attachInterrupt(0, ch1_falling, FALLING);
  prev_CH1_time  = micros();
}
 
void ch1_falling() {
  attachInterrupt(0, ch1_rising, RISING);
  pwm_CH1_value = micros()-prev_CH1_time;
  //Serial.println(pwm_CH1_value);
}


void ch2_rising() {
  attachInterrupt(1, ch2_falling, FALLING);
  prev_CH2_time  = micros();
}
 
void ch2_falling() {
  attachInterrupt(1, ch2_rising, RISING);
  pwm_CH2_value = micros()-prev_CH2_time;
  //Serial.println(pwm_CH2_value);
}
