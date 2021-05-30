/*  FUNCAO: RECEBER CODIGO IR E ESCREVE-LO VIA USB (TECLADO)
 *  
 *  Original  https://www.youtube.com/watch?v=bMEZeX6E1-o
 *  substitui biblioteca TrinketHidCombo pela DigiKeyboard para usar apenas o IR
 *  retorna o codigo do controle no bloco de notas (int) 
 *  ************ APROVADO **************
 *  utiliza numeros inteiros para armazenar o codigo IR
 *  
 *  ATENCAO! DEVIDO A BIBLIOTECA DigiKeyboard, FUNCIONA APENAS NO COMPUTADOR/CELULAR
*/
#include "DigiKeyboard.h"

#define IRpin_PIN PINB // ATTiny85 had Port B pins
#define IRpin 2
#define pinLED 1 //DIGISPARK PIRATA LED = PINO 1

#define MAXPULSE    5000  // the maximum pulse we'll listen for - 5 milliseconds 
#define NUMPULSES    50   // max IR pulse pairs to sample
#define RESOLUTION    2   // time between IR measurements

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[NUMPULSES][2];  // pair is high and low pulse
uint16_t currentpulse = 0;      // index for pulses we're storing
uint32_t irCode = 0;
uint32_t irCodeLast = 0;
unsigned long lastPress = 0;

 //----------------------------------------- CODIGOS CONTROLE SAMSUNG
#define IR_MUTE  4164434947
#define IR_VOLUP 4164433927 
#define IR_VOLDN 4164432907 
#define IR_LEFT  4164430230 
#define IR_RIGHT 4164424110
#define IR_UP    4164420030 
#define IR_DOWN  4164428190
#define IR_SOURC 4164427807
#define IR_POWER 4164423727
/*#define IR_PLAY  0xC02F4837
#define IR_STOP  0xC02F4A35
#define IR_PREV  0xC02F4639
#define IR_NEXT  0xC02F4E31
#define IR_SKIPB 0xC02F700F
#define IR_SKIPF 0xC02F720D
#define IR_REW   0xC02F40BF
#define IR_FFWD  0xC02F641B
#define IR_OK    0xFF38C7 */

void setup() {
  pinMode(IRpin, INPUT);
  DigiKeyboard.sendKeyStroke(0); //inicia a funcao do teclado
  digitalWrite(pinLED, HIGH);
  delay(500);
  digitalWrite(pinLED, LOW);
  delay(500);
  digitalWrite(pinLED, HIGH);
  delay(500);
  digitalWrite(pinLED, LOW);
  delay(500);
  digitalWrite(pinLED, HIGH);
  DigiKeyboard.delay(500);
  digitalWrite(pinLED, LOW);
  DigiKeyboard.println("Hello Digispark!");
  DigiKeyboard.delay(500);
  
}

// ----------------------

void loop()
{
  uint16_t numpulse = listenForIR(); // Wait for an IR Code

  // Process the pulses to get a single number representing code
  for (int i = 0; i < 32; i++) {
    irCode = irCode << 1;
    if ((pulses[i][0] * RESOLUTION) > 0 && (pulses[i][0] * RESOLUTION) < 500) {
      irCode |= 0;
    } else {
      irCode |= 1;
    }
  }

  int repeat = (irCode!=irCodeLast || (millis() - lastPress)>200);
  DigiKeyboard.sendKeyStroke(0);
  DigiKeyboard.println(irCode);
  DigiKeyboard.delay(500);
  switch (irCode) {
    case IR_VOLUP:
      digitalWrite(pinLED, HIGH);
      break;
    case IR_VOLDN:
      digitalWrite(pinLED, LOW);
      break;
    case IR_RIGHT:
      digitalWrite(pinLED, HIGH);
      break;
    case IR_LEFT:
      digitalWrite(pinLED, LOW);
      break;
  }
  irCodeLast = irCode;
  lastPress = millis();

}

// ----------------------

uint16_t listenForIR() {  // IR receive code
  currentpulse = 0;
  while (1) {
    unsigned int highpulse, lowpulse;  // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length

    while (IRpin_PIN & _BV(IRpin)) { // got a high pulse
      highpulse++;
      delayMicroseconds(RESOLUTION);
      if (((highpulse >= MAXPULSE) && (currentpulse != 0)) || currentpulse == NUMPULSES)
        return currentpulse;
      if (lowpulse >= MAXPULSE)
        delay(10);
    }
    pulses[currentpulse][0] = highpulse;

    while (! (IRpin_PIN & _BV(IRpin))) { // got a low pulse
      lowpulse++;
      delayMicroseconds(RESOLUTION);
      if (((lowpulse >= MAXPULSE) && (currentpulse != 0)) || currentpulse == NUMPULSES)
        return currentpulse;
      if (lowpulse >= MAXPULSE)
        delay(10);
    }
    pulses[currentpulse][1] = lowpulse;
    currentpulse++;
  }
}
