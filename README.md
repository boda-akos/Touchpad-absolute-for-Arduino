# Touchpad-absolute-for-Arduino
Using touchpad absolute values  Arduino Nano and STM32
More details on Thingiverse https://www.thingiverse.com/thing:5195666
Touchpad demo, drawing toy

Touchpad could be a cheap and versatile input device for Arduino (source : old laptop. Add two switches for mouse-like operation to select-unselect any position on the TFT screen. 

Parts: Synaptics-like touchpad, Arduino, TFT ST7735 1.8", 5V to 3.3V level shifter 4 units, 10K 20K resistor divider for Reset signal to TFT. STM32 needs no level shift.

This project reads absolute values from Synaptics touchpads, and draws a dot in selectable colors on TFT.

Touchpad typical X Y coordinates range from 950 to 5000, depending on model. This range gets transferred to 128x160 pixels of the TFT for draw. Z coordinate (touch strength) range 0-255. Draws a dot if z>10, light touch. Select draw color on the color bars, clear the image touching the left bottom corner.

Code uses no libraries. Tested on Arduino Nano and STM32 Blue Pill. Connect to T10 clock and T11 data points on touchpad, for me all touchpads worked right away. See Youtube how to find T10 and T11, ground GND and +5V on a touchpad connector, or simply look at the PCB.

Powered via USB. Touchpad works with any voltage 3.3-5V. If you want it portable, use the thick bottom design, there is room for a Li-Ion phone battery, charger PCB and switch.
Use HT7333 to supply 3.3V to the circuit.  For more information about this, see my other projects.

This object was made in Tinkercad. Edit it online https://www.tinkercad.com/things/7IyBmNP5tKZ
