/*
* Raymond Turrisi, 100607022
* MCE 433, Final Project
* 12/18/2020
*
*/
#define greenLED 10 //DIGITAL output for two states
#define yellowLED 13 //DIGITAL output for two states
#define redLED 11 //PWM output for varying brightness
#define buzzer 12 //DIGITAL output for two states
//potIn = A0 //ANALOT INPUT Potentiometer input
#define ss1 2 //
#define ss2 3
#define ss3 4
#define ss4 5
#define ss5 6
#define ss6 7
#define ss7 8
#define buttonIn 9 //DIGITAL INPUT, pullup resistor

void setup() {
  // put your setup code here, to run once:
    pinMode(redLED,OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(buttonIn,INPUT_PULLUP);
    pinMode(A0, INPUT);
    pinMode(ss1,OUTPUT);
    pinMode(ss2,OUTPUT);
    pinMode(ss3,OUTPUT);
    pinMode(ss4,OUTPUT);
    pinMode(ss5,OUTPUT);
    pinMode(ss6,OUTPUT);
    pinMode(ss7,OUTPUT);
    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

}

String buff;

float time_job = 0;
float time_remaining = 0;
float queue_size = 0;
float old_queue_size = 0;
float tmperr = 0;

String tempstr;
int a_read = 0;
int button = 0;
String message;
String local_err_msg;

bool enter_buzzer = false;
bool enter_LEDs = false;
bool enter_ssdisp = false;

void ssDisp(int displayed);
void python_talker();
void python_listener();

void processmgr();
void controlBuzzer() {
  //job completed, long beep then turns off after 2 seconds

  //if it receives a new job, beeps twice

  //if queue is 0, do nothing
}

void viewErr() {
  //pwm out to red led the proportional error, when error is nearly 0, the pwm out is nearly zero, however anything over 20C is bright red
  if(tmperr >= 20) {
    tmperr = 20;
  }
  int pwmOut = 12*tmperr;
  analogWrite(redLED,pwmOut);
}

bool enter_updateSSDisp = false;
void controlssdisp() {
  if(old_queue_size != queue_size) {
    enter_updateSSDisp = true;
  }
  if(enter_updateSSDisp) {
    old_queue_size = queue_size;
    ssDisp(queue_size);
    enter_updateSSDisp = false;
  }
}

bool ledS1Flag = false;
bool ledS2Flag = false;
bool ledS3Flag = false;
bool LEDS_enter_state_1 = false;
bool LEDS_enter_state_2 = false;
bool LEDS_enter_state_3 = false;
bool yellowBlinking = false;

void controlLEDs() {
  float ratio = (float)time_remaining/(float)time_job;
  local_err_msg = String(ratio);
  //if job is less than half of completion time, yellow led is on
  if(ratio <= 0.5 && !ledS1Flag) {
    LEDS_enter_state_1 = true;
    ledS1Flag = true;
  }
  if(LEDS_enter_state_1) {
    digitalWrite(yellowLED,HIGH);
    digitalWrite(greenLED,LOW);
    LEDS_enter_state_1 = false;
  }
  
  //if job is more than half complete, green led is on
  if(ratio >= 0.5 && !ledS2Flag) {
    LEDS_enter_state_2 = true;
    ledS2Flag = true;
  }
  if(LEDS_enter_state_2) {
    digitalWrite(yellowLED,LOW);
    digitalWrite(greenLED,HIGH);
    LEDS_enter_state_2 = false;
  }
  //if queue size is 0, yellow led is blinking periodically
  if(queue_size == 0 && !ledS3Flag) {
    digitalWrite(greenLED,LOW);
    LEDS_enter_state_3 = true;
    ledS3Flag = true;
  }
  float yellowLED_timer = 0;
  if(LEDS_enter_state_3) {
    if(millis() >= yellowLED_timer) {
      digitalWrite(yellowLED,yellowBlinking);
      yellowBlinking = !yellowBlinking;
      yellowLED_timer = millis() + 2000;
    }
    if(queue_size >= 1) {
      ledS1Flag = false;
      ledS2Flag = false;
      ledS3Flag = false;
      LEDS_enter_state_3 = false;
    }
  }
}
void readSensors() {
    a_read = analogRead(A0);
    button = digitalRead(buttonIn); 
}
void loop() {
    python_listener();
    python_talker();   
    readSensors();
    viewErr();
    controlLEDs;
    controlssdisp();
}

float dont_talk_too_much = 0;
void python_talker(){
  if(millis() >= dont_talk_too_much) {
    message = String(button) + " " + String(a_read) + " " + local_err_msg;
    Serial.println(message);
    dont_talk_too_much = millis()+500;
  }
}

void python_listener() {
  if(Serial.available() > 0) {
    int k = 0;
    int i = 0;
    buff = Serial.readStringUntil('\n');
    while(k != 4) {
      if(k == 0) {
        i = buff.indexOf(',');
        tempstr = buff.substring(0,i);
        time_job = tempstr.toFloat();
        buff.remove(0,i+1);
      }else if(k == 1) {
        i = buff.indexOf(',');
        tempstr = buff.substring(0,i);
        time_remaining = tempstr.toFloat();
        buff.remove(0,i+1);
      }else if(k == 2) {
        i = buff.indexOf(',');
        tempstr = buff.substring(0,i);
        queue_size = tempstr.toFloat();
        buff.remove(0,i+1);
      }else if(k == 3) {
        i = buff.indexOf(',');
        tempstr = buff.substring(0,i);
        tmperr = tempstr.toFloat();
        buff.remove(0,i+1);
      }
      k++;
    }
  }
}

void ssDisp(int displayed) {
  switch (displayed) {
    case 0:
      digitalWrite(ss1, LOW);
      digitalWrite(ss2, HIGH);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, HIGH);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, HIGH);
    break;
    case 1:
      digitalWrite(ss1, LOW);
      digitalWrite(ss2, LOW);
      digitalWrite(ss3, LOW);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, LOW);
      digitalWrite(ss6, LOW);
      digitalWrite(ss7, HIGH);
    break;
    case 2:
      digitalWrite(ss1, HIGH);
      digitalWrite(ss2, LOW);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, HIGH);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, LOW);
    break;
    case 3:
      digitalWrite(ss1, HIGH);
      digitalWrite(ss2, LOW);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, LOW);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, HIGH);
    break;
    case 4:
      digitalWrite(ss1, HIGH);
      digitalWrite(ss2, HIGH);
      digitalWrite(ss3, LOW);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, LOW);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, HIGH);
    break;
    case 5:
      digitalWrite(ss1, HIGH);
      digitalWrite(ss2, HIGH);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, LOW);
      digitalWrite(ss5, LOW);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, HIGH);
    break;
    case 6:
      digitalWrite(ss1, HIGH);
      digitalWrite(ss2, HIGH);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, LOW);
      digitalWrite(ss5, HIGH);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, HIGH);
    break;
    case 7:
      digitalWrite(ss1, LOW);
      digitalWrite(ss2, LOW);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, LOW);
      digitalWrite(ss6, LOW);
      digitalWrite(ss7, HIGH);
    break;
    case 8:
      digitalWrite(ss1, HIGH);
      digitalWrite(ss2, HIGH);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, HIGH);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, HIGH);
    break;
    case 9:
      digitalWrite(ss1, HIGH);
      digitalWrite(ss2, HIGH);
      digitalWrite(ss3, HIGH);
      digitalWrite(ss4, HIGH);
      digitalWrite(ss5, LOW);
      digitalWrite(ss6, HIGH);
      digitalWrite(ss7, HIGH);
    break;
  }

  
}
