# OLED-Digital-Dice

This project is based on the work of Joe Coburn for the original design and Fernando Hernandez (Dsk001) for the Wemos version.

In this branch, I fixed the button debouncing and randomizer. Adding battery and shake sensor features.

Sources:
* 2017 Joe Coburn https://www.makeuseof.com/tag/roll-style-diy-electronic-d20/
* 2018 Tomas Carlsson (TomasC62) https://www.instructables.com/Arduino-Oled-Dice/
* 2021 Fernando Hernandez (Dsk001) https://www.prusaprinters.org/prints/66933-electronic-dice

# Hardware Used:

* 1 x D1 Mini (ESP8266, WiFi capable)
* 1 x 0.96" I2C IIC SPI Serial 128x64 OLED screen
* 2 x LED Push Buttons
* 2 x Metal Ball Tilt Switches (optional) https://amzn.to/34mlJKk

For battery operation:

* 1 x Wemos D1 Battery Shield (optional)
* 1 x 500mAh Lipo (optional)
* 1 x LED Latching Push Button

# Printing:

Some openscad files are included in the project.  They assume you have downloaded the source files from https://www.prusaprinters.org/prints/66933-electronic-dice

* oled_face_mod.stl - The front shell oled area is trimmed to make it easier to fit.
* oled_battery_mod.stl - The back shell includes cutaways for the battery shield, second usb connector, and a on/off push button.

# Assembly:

Wire up the following:

 *  D2(SDA), D1(SCL) to OLED
 *  D0 to RST
 *  D3 to Button (ROLL) to Ground
 *  D8 to Button (DIE) to 3.3V
 *  D5 to Tilt Sensor to Ground
 *  D7 to Tilt Sensor to Ground
 *  D6 to Button (ROLL) LED to Ground
 *  D0 to Button (DIE) LED to Ground (I NEED TO MOVE THIS)

If using the Battery Shield and you have v1.2.0 or higher, you can solder jumper J2 to connect the battery to A0 through a 130k resistor.
If not using a battery, comment out #define USE_BATTERY in code.

Test the circuit, then stuff into the 3D printed case.  Use hot glue or similar to affix the OLED.

Glue the tilt switch with the wires facing up, so the dice is only rolled when you shake it on it's side or upside down.
This prevents the number from changing while the dice is just sitting on the table.
