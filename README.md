# DC_nano_temp_control

DC nano temperature controller


Features to add:

Redesign enclosure with USB cutout, reduce size of screw holders to fit Nano between



20240703 Complete assembly and add all files to DC github repo


Operation:
Connect USB power, short display company name (Sharky Thingo and Co.) then show temperature reading and heater output status.
Long press up button - temperature set menu, press up to increase, down to decrease, long press up to return to main screen.
Long press down button - uptime in milliseconds? long press up button to return to main screen.


Assembly:
Arduino Nano:

Powered by USB mini plug

Thermistor 10K NTC
 1 - Gnd
 2 - A0 (10k resistor from A0 to +5V)

Digital output, wired direct to SSR +/-
 1 - Gnd
 2 - D2

Switch inputs 2 x press button
 S1 1 - Gnd
 S1 2 - D3 (up button)
 S2 1 - Gnd
 S2 2 - D4 (down button)

SSD1306 OLED I2C screen 128x32
 1 - Gnd
 2 - 5v
 3 - SCL-to-Nano A5
 4 - SDA-to-Nano A4


Enclosure:
3D printed with PLA silk, design by DC with OnShape
OLED held in position with Hot glue
Nano held in position with 3M VHB double sided tape (enclosure modified with knife and drill to fit)
Switches held in place by bracket secured by M2.3 x 5 STS x 3
Rear cover secured by M2.3 x 5 STS x 4