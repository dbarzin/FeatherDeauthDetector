
FeatherDeauthDetector
=====================

This is a tool for security operation teams to detect Wifi Deauth 
actions made by attackers on they Wifi network.

It is based on Adafruit Feather HUZZAH with ESP8266.

There are two modes : detection mode and bomb mode.

Detection mode
--------------
Detection mode is activated by default and print a message when an attack is detected.
When the attack stop, it restart scanning.
This mode is usefull when an attack is in progress and you want to stop it.

Bomb mode
---------
In the bomb mode, a bomb explose every 120 seconds. 
The bomb stop when the attacker starts a deauth attack.
A manual operation is needed to retsart the counter.
It alows you to known that a death attack did take place.

Operation
---------
Detection mode is activated by default and count the number of second since the last Deauth Attack.
The counter restart by pressing on button "A". Bomb mode start by pressing on button "B".

Hardware
--------
Adafruit Feather HUZZAH with ESP8266 
https://www.adafruit.com/product/2821

Adafruit Quad 2x2 FeatherWing Kit with Headers 
https://www.adafruit.com/product/4253

Assembled Adafruit FeatherWing OLED - 128x32 OLED Add-on For Feather
https://www.adafruit.com/product/3045

Adafruit 15x7 CharliePlex LED Matrix Display FeatherWing Yellow
https://www.adafruit.com/product/3135

Adafruit 0.54" Quad Alphanumeric FeatherWing Display - Yellow
https://www.adafruit.com/product/3131

IKea HÄLSA 
https://www.ikea.com/gb/en/search/products/?q=h%C3%A4lsa

STL File
FeatherDeauthDetectorCase.stl

References
----------

Source based on DeathDetector
https://github.com/spacehuhn/DeauthDetector

and sample code from Adafruit.





