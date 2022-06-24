//control toggling between modes. 
void mode_switch(){
  //check if the mode switch channel is >1700usec for a certain amount of time before switching modes
  int trigger_count = 50; //the number of times we want to see the trigger value before changing modes. 
  int change_threshold = 1700;
  
  
  if (CH1>change_threshold) {
    mode_change_count++;
  }

  if (CH1<change_threshold) {
    mode_change_count = 0;
    mode_changed = 0;  
  }
  
  if (mode_change_count > trigger_count & mode_changed == 0) {
    mode++;
    mode = mode % max_mode; //make the mode rollover back to zero
    mode_change_count = 0;
    mode_changed = 1;
    if (print_serial) {
      Serial.print("Mode changed to: ");
      Serial.println(mode);
    }
  }
  if (print_serial) {
    Serial.print(mode);
    Serial.print("\t");
    Serial.println(mode_change_count);
  }
  


  
}
