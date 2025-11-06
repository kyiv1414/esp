#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

#define IR_RECV_PIN 5
#define IR_SEND_PIN 14

IRrecv irrecv(IR_RECV_PIN);
IRsend irsend(IR_SEND_PIN);
decode_results results;

unsigned long lastSend = 0;
int commandIndex = 0;

struct IRCommand {
  const char* name;
  uint32_t code;
  int bits;
};

IRCommand commands[] = {
  {"TV Power (NEC)", 0x20DF10EF, 32},
  {"TV Vol+ (NEC)", 0x20DF40BF, 32},
  {"TV Vol- (NEC)", 0x20DFC03F, 32},
  {"Samsung Power", 0xE0E040BF, 32}
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\n\n===================================="));
  Serial.println(F("   ESP-01M IR Transceiver Test"));
  Serial.println(F("====================================\n"));
  
  // Ініціалізація
  irrecv.enableIRIn();
  irsend.begin();

}

void loop() {
  
  if (irrecv.decode(&results)) {
    
    Serial.print(F("decode_type:  "));
    Serial.println(typeToString(results.decode_type));
    
    Serial.print(F("value:       0x"));
    serialPrintUint64(results.value, HEX);
    Serial.println();
    
    Serial.print(F("bits:      "));
    Serial.println(results.bits);
    
    Serial.print(F("rawlen:  "));
    Serial.print(results.rawlen - 1);
    Serial.println(F(" імпульсів"));

    Serial.print(F("Raw:  "));
    for (uint16_t i = 1; i < min((uint16_t)17, results.rawlen); i++) {
      Serial.print(results.rawbuf[i] * kRawTick);
      if (i < (uint16_t)min(16, (int)results.rawlen - 1)) Serial.print(F(", "));
    }
    if (results.rawlen > 17) Serial.print(F("..."));
    Serial.println();
    
    Serial.println(F("════════════════════════════════\n"));
    
    irrecv.resume();
  }
  
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    
    IRCommand cmd = commands[commandIndex];
    
    Serial.print(F("name:   "));
    Serial.println(cmd.name);
    Serial.print(F("code:       0x"));
    Serial.println(cmd.code, HEX);
    Serial.print(F("bits:      "));
    Serial.println(cmd.bits);
    
    irsend.sendNEC(cmd.code, cmd.bits);
    
    Serial.println(F("Success"));
    Serial.println(F("════════════════════════════════\n"));

    commandIndex = (commandIndex + 1) % 4;

    irrecv.enableIRIn();
  }
  
  delay(100);
}