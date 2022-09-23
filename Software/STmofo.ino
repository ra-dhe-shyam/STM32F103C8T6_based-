
float APPS1_Voltage, APPS2_Voltage, BrakeSensor_Voltage, APPS1_PedalTravel, APPS2_PedalTravel,  Delta, BSPD_OP, z = 0.6;
#define APPS_SCSpin 5
#define Start_Buttonpin 6
#define AIR_AUXpin 7
#define RTDS_CTRLpin 8
#define APPS_CTRLpin 9
#define Brakelight_CTRLpin 10
int BrakeSensor_Reading, APPS1_newReading, APPS1_currentReading, APPS1_lastReading, APPS2_newReading, APPS2_currentReading, APPS2_lastReading, timer = 1,RTD = 0,AIR_Check; 
bool error1 = true;
bool error2 = true;
void Impl1();
void Impl2();
void RTDS();
void Read_APPS_Data();
void Filter_APPS();

void setup() {
  RTD=0;
  Serial.begin(115200);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(Start_Buttonpin,INPUT);
  pinMode(AIR_AUXpin,INPUT);
  pinMode(APPS_SCSpin,OUTPUT);
  pinMode(APPS_CTRLpin,OUTPUT);
  pinMode(RTDS_CTRLpin,OUTPUT);
  pinMode(Brakelight_CTRLpin,OUTPUT);
  digitalWrite(APPS_SCSpin,1);
  
  
}
void Read_APPS_Data(){
  APPS1_newReading = analogRead(A0);
  APPS2_newReading = analogRead(A1);
  APPS1_PedalTravel = 100.0 * ((APPS1_newReading)/(1023.0));  //substitute 5 and 1.1 value of APPS1_Voltage voltage range
  APPS2_PedalTravel = 100.0 * ((APPS2_newReading)/(1023.0));    //substitute 8.192 and 1.5 value of APPS2_Voltage voltage range
  Delta = abs(APPS1_PedalTravel - APPS2_PedalTravel);
}

void Filter_APPS(){
  APPS1_lastReading = analogRead(A0);
  APPS2_lastReading = analogRead(A1);
};

void RTDS(){
  AIR_Check = digitalRead(AIR_AUXpin);
  int Button = digitalRead(Start_Buttonpin);
  BrakeSensor_Reading = analogRead(A4);
  if((AIR_Check==1)&&(Button==1)&&(int(BrakeSensor_Reading >= 140))){
    digitalWrite(RTDS_CTRLpin,1);
    delay(1500);
    digitalWrite(RTDS_CTRLpin,0);
    RTD = 1;
    }
  else{
    RTD = 0;
  }
}


void Brakelight_ON(){
  BrakeSensor_Reading = analogRead(A4);
  if(BrakeSensor_Reading > 140){
   digitalWrite(Brakelight_CTRLpin,1);
  }
  else{
   digitalWrite(Brakelight_CTRLpin,0);
  }
}


void loop() {
  AIR_Check = digitalRead(AIR_AUXpin);
  if(!RTD){
    RTDS();
  }
  if(AIR_Check && RTD){
    error1 = false;
    error2 = false;
    digitalWrite(APPS_CTRLpin,0);
    Impl1();
    Impl2();
    if(error1||error2){
      digitalWrite(APPS_CTRLpin,1);
      if(((APPS1_currentReading > (APPS1_lastReading-10)) || (APPS1_currentReading<(APPS1_lastReading+10))) && (APPS2_currentReading != APPS2_lastReading)){
        while(Delta>=10.0){
           digitalWrite(APPS_SCSpin,0); 
        }
      }
    }
    else{
      digitalWrite(APPS_CTRLpin,0);
    }
  }
  else{
    digitalWrite(APPS_CTRLpin,1);
    RTD = 0;
  }
}
void Impl1(){
  Filter_APPS();
  Read_APPS_Data();
  if(Delta >= 10){
    int impl1 = millis();
    while(Delta >= 10){
      Filter_APPS();
      Read_APPS_Data();
      if((millis()-impl1)>100){
          error1 = true;
          break;
      }
    }
  }
  else {
    error1 = false;
  }
}

void Impl2(){
  BSPD_OP = analogRead(A3);
  if( BSPD_OP > 716 ){
    int impl2 = millis();
    while(BSPD_OP > 716){
      BSPD_OP = analogRead(A3);
      if((millis()-impl2)>500){
        error2 = true;
        break; 
      }
    }
    while(APPS1_PedalTravel > 5.0){
      digitalWrite(APPS_CTRLpin,1);
      Filter_APPS();
      Read_APPS_Data();
      Serial.println(APPS1_PedalTravel);
    }
  }
  else {
    error2 = false;
  }
}
