#include<SoftwareSerial.h>

//Baudot lookup table
unsigned char lookup[] = { 
  ' ',     'E',    ' ',     'A',    ' ',     'S',    'I',     'U',
  '\n',    'D',    'R',     'J',    'N',     'F',    'C',     'K',
  'T',     'Z',    'L',     'W',    'H',     'Y',    'P',     'Q',
  'O',     'B',    'G',     0,      'M',     'X',    'V',     0, 
  ' ',     '3',    '\n',    '-',    ' ',     '`',    '8',     '7',
  '\r',    ' ',    '4',     '\'',   ',',     '!',    ':',     '(',
  '5',     '+',    ')',     '2',    '$',     '6',    '0',     '1',
  '9',     '?',    '&',     0,      '.',     '/',    ';',     0    };
  
const int RELAY_PIN = 7;
const int CHAR_DELAY = 22;       //milliseconds
const int START_BIT_DELAY = 22;  // milliseconds
const int STOP_BIT_DELAY = 44;   // milliseconds
const int SPACE = 4;
const int F_SHIFT = 27;
const int L_SHIFT = 31;
const int CR = 8;
const int LF = 2;
  
boolean letters_on = true;
boolean print_debug = false;

/* transmits 5 bit integers */
void byte_send(int _byte){

    send_start_bit();  
    
    // loop through all 5 bits and send a space for zero bits, and mark for one bits.
    for(int _bit = 0; _bit < 5; _bit++){
      
        switch (bitRead(_byte,_bit)) {
          
          case 0:
              Serial.print(0);
              space();
              break;
              
          case 1:
              Serial.print(1);
              mark();
              break;
        }  // end switch
    }  // end for loop
    
    send_stop_bit();
    
} // end byte_send()

/* Lookup and decode characters before transmitting. */
void char_send(char letter){
    char l = toupper(letter);
    Serial.print("sending ");
    Serial.print(l);
    int idx = lookupChar(l);
    Serial.print("   idx: ");
    Serial.print(idx);  
    Serial.print("  Binary: ");
    // check for offset  
    if (31 < idx){          // shift for figures
        
        if (letters_on){
            figure_shift();
        }
        idx = idx - 32;
        
      } else {                // shift for letters
         if (false == letters_on){
             letter_shift();
         } 
      }
      
      byte_send(idx);
      Serial.println();
} // end char_send()

void letter_shift(){ 
    Serial.print(" [L_SHIFT] ");
    letters_on = true;
    byte_send(L_SHIFT); 
}

void figure_shift(){ 
  Serial.print(" [F_SHIFT] ");
  letters_on = false;
  byte_send(F_SHIFT); 
}

void cr(){ Serial.print(" <CR> "); byte_send(CR); }

void lf(){ Serial.print(" <LF>"); byte_send(LF); }

void send_start_bit(){
    digitalWrite(RELAY_PIN, HIGH);
    delay(START_BIT_DELAY);  
}

void send_stop_bit(){
    digitalWrite(RELAY_PIN, LOW);
    delay(STOP_BIT_DELAY);
}

void mark(){
    digitalWrite(RELAY_PIN, LOW);
    delay(CHAR_DELAY);
}

void space(){
    digitalWrite(RELAY_PIN, HIGH);
    delay(CHAR_DELAY);
}

int lookupChar(char letter){
    if (' ' == letter){ return 4; }
    for (int lookupIdx = 0; lookupIdx < sizeof(lookup); lookupIdx++){
       if (letter == lookup[lookupIdx]){
           return lookupIdx;
       } 
    }
    return 0;
}

void setup(){
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(5, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
}

void demo(){
  cr();
  lf();
  letter_shift();
  for (int i = 0; i < 34; i++){
    char_send('R');
    char_send('Y');
  } 
}

void demo2(){
  int idx = 0;
  char demo_string[] = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG'S BACK 1234567890 TIMES";
  cr();
  lf();
  letter_shift();
  for (int i = 0; i < sizeof(demo_string); i++){
     char_send(demo_string[i]); 
  }
}

void demo3(){
  
  int cur = 0;
  
  while (cur < 68) {
      if (Serial.available() > 0) {
          Serial.print(cur);
          Serial.print(": ");
          char_send(Serial.read());
          cur++;
      }
  }
  cr();
  lf();
}

void loop() {
  //demo();
  //demo2();
  demo3();
}
