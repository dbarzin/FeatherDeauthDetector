
// (¯`·._.·(¯`·._.·---------------------·´¯)·._.·´¯)·._.·´¯)
// (¯`·._.·(¯`·._.· FeatherDeathDetector ´¯)·._.·´¯)·._.·´¯)
// (¯`·._.·(¯`·._.·---------------------·´¯)·._.·´¯)·._.·´¯)

// v1.0 by dbarzin 12Sept2019 


#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_IS31FL3731.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment seg7Matrix = Adafruit_7segment();
Adafruit_IS31FL3731_Wing ledMatrix = Adafruit_IS31FL3731_Wing();
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// ===== SETTINGS ===== //
#define CH_TIME 140        /* Scan time (in ms) per channel */
#define PKT_RATE 5         /* Min. packets before it gets recognized as an attack */
#define PKT_TIME 2         /* Min. interval (CH_TIME*CH_RANGE) before it gets recognized as an attack */

// OLED FeatherWing buttons map to different pins depending on board:
#define BUTTON_A  0
#define BUTTON_B 16
#define BUTTON_C  2

// Action counter
uint16_t counter = 0;
bool attack = false;
bool bomb_mode = false;
bool bomb_stopped = false;

// led matrix brightness changes to be more visible
uint8_t sweep[] = {1, 2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 60, 60, 40, 30, 20, 15, 10, 8, 6, 4, 3, 2, 1};

// Cursor loop
char cursor[4] = {'|','/','-','\\'};

// Channels to scan on (US=1-11, EU=1-13, JAP=1-14)
const short channels[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13/*,14*/ };

// ===== Runtime variables ===== //
int ch_index = 0 ;               // Current index of channel array
int packet_rate = 0 ;            // Deauth packet counter (resets with each update)
int attack_counter = 0 ;         // Attack counter
unsigned long update_time = 0 ;  // Last update time
unsigned long ch_time = 0 ;      // Last channel hop time

// ===== Sniffer function ===== //
void sniffer(uint8_t *buf, uint16_t len) {
    if (!buf || len < 28) return; // Drop packets without MAC header

    byte pkt_type = buf[12]; // second half of frame control field
    //byte* addr_a = &buf[16]; // first MAC address
    //byte* addr_b = &buf[22]; // second MAC address

    // If captured packet is a deauthentication or dissassociaten frame
    if (pkt_type == 0xA0 || pkt_type == 0xC0) {
        ++packet_rate;
    }
}

// ===== Attack detection functions ===== //
void attack_started() {
    // digitalWrite(LED, !LED_INVERT); // turn LED on
    // Serial.println("!!! ATTACK DETECTED !!!");
    messageDisplay("! DEAUTH !");
    attack=true;
}

void attack_stopped() {
    // digitalWrite(LED, LED_INVERT); // turn LED off
    messageDisplay("Scanning...");
    attack=false;
    counter=0;
}


// ===== SETUP ===== //
void setup() {
    ledMatrix.begin();
    ledMatrix.setRotation(0);

    seg7Matrix.begin(0x70);

    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

    // Clear the buffer.
    display.clearDisplay();
    display.display();

    // text display tests
    messageDisplay("Scaning...");

    WiFi.disconnect();                   // Disconnect from any saved or active WiFi connections
    wifi_set_opmode(STATION_MODE);       // Set device to client/station mode
    wifi_set_promiscuous_rx_cb(sniffer); // Set sniffer function
    wifi_set_channel(channels[0]);        // Set channel
    wifi_promiscuous_enable(true);       // Enable sniffer
}

// ===== Display a message on LCD ==== //
void messageDisplay(char *str) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(str);
    display.display(); // actually display all of the above
}

// ===== Main Loop ===== //
void loop() {
    // Check Buttons
    if(!digitalRead(BUTTON_A)) {
        messageDisplay("Scaning...");
        bomb_mode=false;
        bomb_stopped=false;
        counter=0;
    }

    if(!digitalRead(BUTTON_B)) {
        messageDisplay("BOMB ARMED");
        bomb_mode=true;
        bomb_stopped=false;
        counter=120;
    }

    if (!bomb_stopped) {
        // cursor loop
        if (!attack) {
            display.fillRect(0, 15, 15, 24, BLACK);
            display.setCursor(0,18);
            display.print(cursor[counter%4]);
            display.display(); // actually display all of the above
        }

        // led matrix
        if (attack) {
            if (bomb_mode) {
                messageDisplay("BOMB STOP! ");
                ledMatrix.clear();
                bomb_stopped=true;
                attack=false;
            }
            else
                for (uint8_t x = 0; x < 16; x++)
                    for (uint8_t y = 0; y < 9; y++)
                        ledMatrix.drawPixel(x, y, sweep[(x+y+counter)%24]/2);
        }
        else {
            // save power - clear previous line
            if (bomb_mode) {
                if ((counter%15)==14)
                    ledMatrix.clear();
            }
            else if ((counter%15)==0)
                ledMatrix.clear();
            ledMatrix.drawPixel(counter%15,(counter/15)%7,1);
        }

        // 7seg Display
        if (attack) {
            if (bomb_mode) {
                if ((counter%2)==0)
                    seg7Matrix.println(counter);
                else
                    seg7Matrix.clear();
            }
            else if ((counter%2)==0)
                seg7Matrix.println(8888);
            else
                seg7Matrix.clear();
            seg7Matrix.writeDisplay();
        }
        else {
            seg7Matrix.writeDigitNum(0, (counter / 4000), false);
            seg7Matrix.writeDigitNum(1, (counter / 400) % 10, false);
            seg7Matrix.writeDigitNum(3, (counter / 40) % 10, false);
            seg7Matrix.writeDigitNum(4, counter / 4 % 10, false);
            seg7Matrix.writeDisplay();
        }

        // update counter
        if (bomb_mode) {
            counter--;
            // BOOM
            if (counter==0) {
                messageDisplay("!! BOOM !!");
                for (uint8_t i=0; i<128; i++) {
                    for (uint8_t x = 0; x < 16; x++) {
                        for (uint8_t y = 0; y < 9; y++) {
                            ledMatrix.drawPixel(x, y, sweep[(x+y+i)%24]/2);
                        }
                    }
                    if ((i%2)==0)
                        seg7Matrix.println(8888);
                    else
                        seg7Matrix.clear();
                    seg7Matrix.writeDisplay();
                    delay(100);
                }
                // clear matrix
                ledMatrix.clear();
                // Wait a little
                delay(1000);
                // Restart bomb
                messageDisplay("BOMB ARMED");
                seg7Matrix.clear();
                counter=120;
            }
        }
        else
            counter ++;

        // Sleep
        delay(240);

        // ===== Wifi scanning ===== //

        unsigned long current_time = millis(); // Get current time (in ms)

        // Update each second (or scan-time-per-channel * channel-range)
        if (current_time - update_time >= (sizeof(channels)*CH_TIME)) {
            update_time = current_time; // Update time variable

            // When detected deauth packets exceed the minimum allowed number
            if (packet_rate >= PKT_RATE) {
                ++attack_counter; // Increment attack counter
            } else {
                if(attack_counter >= PKT_TIME) attack_stopped();
                attack_counter = 0; // Reset attack counter
            }

            // When attack exceeds minimum allowed time
            if (attack_counter == PKT_TIME) {
                attack_started();
            }

            packet_rate = 0; // Reset packet rate
            
        }

        // Channel hopping
        if (sizeof(channels) > 1 && current_time - ch_time >= CH_TIME) {
            ch_time = current_time; // Update time variable

            // Get next channel
            ch_index = (ch_index+1) % (sizeof(channels)/sizeof(channels[0]));
            short ch = channels[ch_index];

            // Set channel
            wifi_set_channel(ch);
        }
    }
}
