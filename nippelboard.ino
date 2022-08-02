#include <SPI.h>
#include "RF24.h"
#include "Bounce2.h"

const uint8_t BUTTON_PINS[] = {2, 3, 4, 5, 6, 7, 8};

#define VOLUME_PIN A0
#define VCC_PIN A3

RF24 radio(9,10);
Bounce2::Button buttons[7];

const byte address[13] = "NippelBoard1";

struct ButtonState {
  uint8_t pressed;
  uint8_t volume;
} buttonState;

void setup() {

  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);

  for(int8_t i = 0; i < 7; i++){
    buttons[i] = Bounce2::Button();
    buttons[i].attach( BUTTON_PINS[i] ,  INPUT_PULLUP );
    buttons[i].interval(5);
    buttons[i].setPressedState(LOW);  
  }
  
  pinMode(VCC_PIN, OUTPUT);
  digitalWrite(VCC_PIN, HIGH);
}

uint8_t last_volume = 0;

void sendMessage() {
  radio.write(&buttonState, sizeof(buttonState));
}

void loop() {

  for(uint8_t i = 0; i < 7; i++){
    buttons[i].update();
    if(buttons[i].pressed()){
      buttonState.pressed = (1 << i);
      sendMessage();
      buttonState.pressed = 0;
    }
  }

  uint8_t volume = map(analogRead(VOLUME_PIN), 0, 1023, 0, 255);

  if(abs(last_volume - volume) > 4){
    last_volume = volume;
    buttonState.volume = volume;
    sendMessage();
  }
}
