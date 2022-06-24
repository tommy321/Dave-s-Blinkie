// You can choose the latch pin yourself.
const int ShiftPWM_latchPin=7;


// If your LED's turn on if the pin is low, set this to true, otherwise set it to false.
const bool ShiftPWM_invertOutputs = false;

// You can enable the option below to shift the PWM phase of each shift register by 8 compared to the previous.
// This will slightly increase the interrupt load, but will prevent all PWM signals from becoming high at the same time.
// This will be a bit easier on your power supply, because the current peaks are distributed.
const bool ShiftPWM_balanceLoad = false;

#define SHIFTPWM_USE_TIMER3
#include <ShiftPWM.h>   // include ShiftPWM.h after setting the pins!

// Here you set the number of brightness levels, the update frequency and the number of shift registers.
// These values affect the load of ShiftPWM.
// Choose them wisely and use the PrintInterruptLoad() function to verify your load.
unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 75;
unsigned int numRegisters = 1;
unsigned int numOutputs = numRegisters*8;
unsigned int numRGBLeds = numRegisters*8/3;
unsigned int fadingMode = 0; //start with all LED's off.
unsigned long startTime = 0; // start time for the chosen fading mode

//LED control variables
int brightness = 0; //variable for brightness
int mode = 0; //flag to track what the current mode is. 
int max_mode = 4;  //number of different flash modes.
int mode_change_count = 0; //counter for debouncing the mode switch. 
int mode_changed = 0;


//radio stuff
#define CH1_pin 2
#define CH2_pin 3
//interrupts for reading the RC radio data. 
volatile int pwm_CH1_value = 0;
volatile int prev_CH1_time = 0;
volatile int pwm_CH2_value = 0;
volatile int prev_CH2_time = 0;

//radio values:
int CH1 = 1500;
int CH2 = 1500;
int CH1_min = 1200;
int CH2_min = 1200;
int CH1_max = 1800;
int CH2_max = 1800;

bool print_serial = 1;

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
//int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
// int ch = pulseIn(channelInput, HIGH, 30000);
//  if (ch < 100) return defaultValue;
//  
  //return ch;
// return map(ch, 680, 1600, minLimit, maxLimit);
//}




void setup() {
  if (print_serial) {
    Serial.begin(115200);
  }
  // Sets the number of 8-bit registers that are used.
  ShiftPWM.SetAmountOfRegisters(numRegisters);

  // SetPinGrouping allows flexibility in LED setup. 
  // If your LED's are connected like this: RRRRGGGGBBBBRRRRGGGGBBBB, use SetPinGrouping(4).
  ShiftPWM.SetPinGrouping(1); //This is the default, but I added here to demonstrate how to use the funtion
  
  ShiftPWM.Start(pwmFrequency,maxBrightness);
  //turn off all the lights to start
  ShiftPWM.SetAll(0);
  pinMode(CH1_pin, INPUT);
  pinMode(CH2_pin, INPUT);
  attachInterrupt(0, ch1_rising, RISING);
  //attachInterrupt(0, ch1_falling, FALLING);
  attachInterrupt(1, ch2_rising, RISING);
  // attachInterrupt(1, ch2_falling, FALLING);
}

void loop() {
  //process the radio stuff. 
  process_radio();
  //Serial.print(CH1);
  //Serial.print("\t");
  //Serial.println(CH2);
  //delay(100);
  brightness = map(CH2, CH2_min, CH2_max, 10, 254);
  //Serial.println(brightness);
  mode_switch();
  
  switch(mode) {
    case 0: //all off
      ShiftPWM.SetAll(0);
      break;
    case 1: //all on... brightness from channel 2
      ShiftPWM.SetAll(brightness);
      break;
    case 2: //have some defined static patterns. Channel 2 selects the pattern
      defined_pattern();
      break;
    case 3:
      selectLED();//make this one random flashing. 
      break;
      
  }
  //ShiftPWM.PrintInterruptLoad();
}


void defined_pattern(void){
  #define num_patterns 5 //How many patterns we want to define.
  int pattern[num_patterns][8] = {{255, 255, 255, 255, 255, 255, 255, 255},
                                  {255, 0, 0, 0, 0, 0, 0, 255},
                                  {0, 255, 0, 0, 0, 0, 255, 0},
                                  {0, 0, 255, 0, 0, 255, 0, 0},
                                  {0, 0, 0, 255, 255, 0, 0, 0},
                                  };
  int selected_pattern = map(CH2, CH2_min, CH2_max, 0, num_patterns);                              
  for (int i = 0; i<8; i++) {
    ShiftPWM.SetOne(i, pattern[selected_pattern][i]);
  }
  
}


void selectLED(void) {
  //use the CH2 input to select which LED is on
  int activeLED = map(CH2, CH2_min, CH2_max, 0, numOutputs);
  ShiftPWM.SetAll(0);
  ShiftPWM.SetOne(activeLED, 255);
}

void oneByOne(void){ // Fade in and fade out all outputs one at a time
  unsigned char brightness;
  unsigned long fadeTime = map(CH2, CH2_min, CH2_max, 10, 200);
  unsigned long loopTime = numOutputs*fadeTime*2;
  unsigned long time = millis()-startTime;
  unsigned long timer = time%loopTime;
  unsigned long currentStep = timer%(fadeTime*2);

  int activeLED = timer/(fadeTime*2);

  if(currentStep <= fadeTime ){
    brightness = currentStep*maxBrightness/fadeTime; ///fading in
  }
  else{
    brightness = maxBrightness-(currentStep-fadeTime)*maxBrightness/fadeTime; ///fading out;
  }
  ShiftPWM.SetAll(0);
  ShiftPWM.SetOne(activeLED, brightness);
}

void inOutAll(void){  // Fade in all outputs
  unsigned char brightness;
  unsigned long fadeTime = map(CH2, CH2_min, CH2_max, 10, 2000);
  unsigned long time = millis()-startTime;
  unsigned long currentStep = time%(fadeTime*2);
  if(currentStep <= fadeTime ){
    brightness = currentStep*maxBrightness/fadeTime; ///fading in
  }
  else{
    brightness = maxBrightness-(currentStep-fadeTime)*maxBrightness/fadeTime; ///fading out;
  }
  ShiftPWM.SetAll(brightness);
}
