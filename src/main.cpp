#include "InIndKit.h"
#include <Arduino.h>

#define TRMNivel def_pin_R4a20_1
#define TRMVazao def_pin_R4a20_2
#define MOTBomba def_pin_RELE
#define CHNivel def_pin_D1
#define POSValvula 0

bool tripValue = false;

AsyncDelay_c delayPOT(50); // time mili second
void monitoraPOT(void) {
  if (delayPOT.isExpired()){
    delayPOT.repeat();

    const uint16_t vlPOT1 = analogRead(def_pin_POT1);
    ledcWrite(POSValvula, vlPOT1);    
    const uint16_t vlPOT2 = analogRead(def_pin_POT2);

    InIndKit.setDisplayText(2, ("P1:" + String(vlPOT1) + "  P2:" + String(vlPOT2)).c_str());
    
    WSerial.plot("vlPOT1", vlPOT1);
    WSerial.plot("vlPOT2", vlPOT2);
  }
}

//https://portal.vidadesilicio.com.br/controle-de-potencia-via-pwm-esp32/#:~:text=Esta%20função%20configura%20um%20canal,Resolução%3A%201%20–%2016%20bits.
AsyncDelay_c delay4A20(50); // time mili second
void monitora4A20(void) {
  if (delay4A20.isExpired()) {
    delay4A20.repeat();

    const double vlR4a20_1 = 16.0*analogRead(def_pin_R4a20_1)/4096.0+4.0;
    const double vlR4a20_2 = 16.0*analogRead(def_pin_R4a20_2)/4096.0+4.0;

    InIndKit.setDisplayText(3, ("T1:" + String(vlR4a20_1) + "  T2:" + String(vlR4a20_2)).c_str());
    
    WSerial.plot("vlR4a20_1", vlR4a20_1);
    WSerial.plot("vlR4a20_2", vlR4a20_2);
  }
}

void IRAM_ATTR trip_func() {
  digitalWrite(MOTBomba,LOW);
  tripValue = true;
}

void setup() {
  InIndKit.setup(115200);
  
  WSerial.onInputReceived([](String str) {
    if(str == "^q") WSerial.telnetStop(); 
    else WSerial.println(str); 
    }
  );
  
  pinMode(CHNivel, INPUT_PULLDOWN);
  attachInterrupt(CHNivel, trip_func, RISING);

  ledcAttachPin(def_pin_W4a20_1, POSValvula);//Atribuimos o pino def_pin_W4a20_1 ao canal POSValvula.
  ledcSetup(POSValvula, 19000, 12);//Atribuimos ao canal 0 a frequencia de 78kHz com resolucao de 10bits.
  ledcWrite(POSValvula, 0);//Escrevemos um duty cycle de 0% no canal 0.

  rtn_1.onValueChanged([](uint8_t status) {
      if(!tripValue) digitalWrite(MOTBomba,status);
      WSerial.println(status? "MOTBomba ON" :"MOTBomba OFF"); 
    }
  );
}

void loop() {
  InIndKit.loop();
  monitoraPOT();
  monitora4A20();
}