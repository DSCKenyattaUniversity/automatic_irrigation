#include <arduino.h>
// *** Soil moisture sensor ***
#define moisture_sensor_power 7
#define moisture_sensor_ctrlpin A0

#define soil_wet 400
#define soil_dry 950

// *** Relay ***
#define relay_control_pin 6

// *** stators LEDs 
#define code_green 5
#define code_red 4
#define mode_led 3
#define toggle_mode_btn 2
#define water_btn 8



// *** functions ***
bool auto_watering_mode = false;
bool manual_watering_mode = false;
bool manual_function_switch = true;

void water (){
  digitalWrite(relay_control_pin, LOW);
  digitalWrite(code_red, LOW);
  digitalWrite(code_green, HIGH);
  delay(100);
  digitalWrite(code_green, LOW);
  delay(100);
}

void stop_watering(){
   digitalWrite(relay_control_pin, HIGH);
  
}

int read_moisture_sensor(){
  digitalWrite(moisture_sensor_power, HIGH);
  delay(1000);
  int moisture_sensor_value = analogRead(moisture_sensor_ctrlpin);
  digitalWrite(moisture_sensor_power, LOW);
  return moisture_sensor_value;
} 

void notify (int moisture_reading, String message) {
  Serial.print("Notification: ");
  Serial.print(message);
  Serial.print(",  ");
  Serial.print(moisture_reading);
  Serial.print("\n\n");
}

void toggle_watering_mode(){  
  if(auto_watering_mode){
    auto_watering_mode = false;
    digitalWrite(mode_led, LOW); 
  }else{
    auto_watering_mode = true;
    manual_watering_mode =false;
    digitalWrite(mode_led, HIGH); 
  }
  
    Serial.print("autowatering mode changed!");
    Serial.println(auto_watering_mode);
    stop_watering();
}

void set_manual_watering(int soil_moisture){ 
 
    if(manual_function_switch){
        if(manual_watering_mode){
        auto_watering_mode = false;
        manual_watering_mode = false;
        stop_watering();
        digitalWrite(code_green, LOW);
        digitalWrite(mode_led, LOW);
        }else{
          manual_watering_mode =true;
        }
    }else{
      
    }
 }


void setup() {
 pinMode(moisture_sensor_power, OUTPUT);
 pinMode(code_green, OUTPUT);
 pinMode(code_red, OUTPUT);
 pinMode(mode_led, OUTPUT);
 digitalWrite(moisture_sensor_power, LOW);
 pinMode(relay_control_pin, OUTPUT);
 pinMode(toggle_mode_btn, INPUT_PULLUP);
 pinMode(water_btn, INPUT_PULLUP);

 stop_watering();
 Serial.begin(9600);
}

void loop() {

//**** get moisture sensor reading ****
  int moisture_reading = read_moisture_sensor();

// **** button inputes ****
  int mode_btn_value = digitalRead(toggle_mode_btn);
  int water_btn_value = digitalRead(water_btn);

  if(mode_btn_value == 0){
    toggle_watering_mode();
    Serial.println(mode_btn_value);
  }

// **** water button reaction ****
  if(water_btn_value == 0 ){
    set_manual_watering(moisture_reading); 
  }

  if(manual_watering_mode){
    water();
  }


// ***** do something depending on the moisture sensor reading ****
  if (moisture_reading < soil_wet) {
    notify(moisture_reading, "soil moisture above average");
    stop_watering();
    digitalWrite(code_green, LOW);
    digitalWrite(code_red, HIGH);
    delay(1000);
    digitalWrite(code_red, LOW);
    delay(100);
    manual_function_switch = false;
  }else if (moisture_reading >= soil_wet && moisture_reading < soil_dry) {
    stop_watering();
    notify(moisture_reading, "Status: Soil moisture is perfect");
    digitalWrite(code_green, HIGH);
    digitalWrite(code_red, LOW);   
    manual_function_switch = false; 
  } else {
    manual_function_switch = true;
    if(auto_watering_mode){
      water();
      notify(moisture_reading, "soil moisture below required, automatic watering on");
    }else if(manual_watering_mode){
      water();
      notify(moisture_reading, "soil moisture below required, manual watering turned on!!");
    }else{
      notify(moisture_reading, "Soil moisture below required, automatic watering off");
      digitalWrite(code_red, HIGH);
      digitalWrite(code_green, LOW);
      delay(1000);
    }
    
  }

// **** take the moisture sensor reading every 1 second
delay (1);
}
