// ATMEGA328 for ATmega328 (Uno, Mega, Nano)
// DEBUG for the serial monitor
#define ATMEGA328 1

#ifdef ATMEGA328
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

// BUTTONS
const int N_BUTTONS = 3;
const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {22, 24, 26};

int buttonCState[N_BUTTONS] = {};
int buttonPState[N_BUTTONS] = {};

// uncomment if you are using pin 13
// index of the pin 13 of the buttonPin[] array
// #define pin13 1
// byte pin13index = 12;

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};
// debounce time; increase if  output flickers
unsigned long debounceDelay = 5;


// POTENTIOMETERS
const int N_POTS = 2;
const int POT_ARDUINO_PIN[N_POTS] = {A0, A1};

int potCState[N_POTS] = {0};
int potPState[N_POTS] = {0};
int potVar = 0;

int midiCState[N_POTS] = {0};
int midiPState[N_POTS] = {0};

// amount of time the potentiometer will be read after it exceeds the varThreshold
const int TIMEOUT = 300;
// threshold for the potentiometer signal variation
const int varThreshold = 50;
boolean potMoving = true;
unsigned long PTime[N_POTS] = {0};
unsigned long timer[N_POTS] = {0};

// MIDI
// MIDI channel to be used
byte midiCh = 1;
// lowest note to be used 
byte note = 36;
// lowest MIDI CC to be used
byte cc = 1;

// SETUP
void setup() {

    // Baud Rate
    // 31250 for MIDI class compliant, 115200 for Hairless MIDI
    Serial.begin(115200);

    #ifdef DEBUG
    Serial.println("Debug mode");
    Serial.println();
    #endif

    // Buttons
    for (int i = 0; i < N_BUTTONS; i++) {
        pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
    }

    #ifdef pin13
    pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
    #endif

}

// LOOP
void loop() {

    buttons();
    potentiometers();

}

// BUTTONS
void buttons() {

    for (int i = 0; i < N_BUTTONS; i++) {
        buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);

        #ifdef pin13
        if (i == pin13index) {
            buttonCState[i] = !buttonCState[i];
        }
        #endif

        if ((millis() - lastDebounceTime[i]) > debounceDelay) {
            if (buttonPState[i] != buttonCState[i]) {
                lastDebounceTime[i] = millis();
                if (buttonCState[i] == LOW) {
                    #ifdef ATMEGA328
                    MIDI.sendNoteOn(note + i, 127, midiCh);
                    #elif DEBUG
                    Serial.print(i);
                    Serial.println(": button on");
                    #endif
                } else {
                    #ifdef ATMEGA328
                    // use if using with ATmega328 (uno, mega, nano...)
                    // note, velocity, channel
                    MIDI.sendNoteOn(note + i, 0, midiCh);
                    #elif DEBUG
                    Serial.print(i);
                    Serial.println(": button off");
                    #endif
                }
                buttonPState[i] = buttonCState[i];
            }
        }
    }
}

// POTENTIOMETERS
void potentiometers() {

    for (int i = 0; i < N_POTS; i++) {
        potCState[i] = analogRead(POT_ARDUINO_PIN[i]);
        midiCState[i] = map(potCState[i], 0, 1023, 0, 127);
        potVar = abs(potCState[i] - potPState[i]);

        if (potVar > varThreshold) { 
            PTime[i] = millis();
        }

        timer[i] = millis() - PTime[i];

        if (timer[i] < TIMEOUT) {
            potMoving = true;
        } else {
            potMoving = false;
        }

        if (potMoving == true) {
            if (midiPState[i] != midiCState[i]) {
            
                #ifdef ATMEGA328
                MIDI.sendControlChange(cc + i, midiCState[i], midiCh);
                #elif DEBUG
                
                Serial.print("Pot: ");
                Serial.print(i);
                Serial.print(" ");
                Serial.println(midiCState[i]);
                //Serial.print("  ");
                #endif
                
                potPState[i] = potCState[i];
                midiPState[i] = midiCState[i];
            }
        }
    }
}
