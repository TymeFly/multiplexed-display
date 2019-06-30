# multiplexed-display
An updated version of the Ben Eater multiplexed-display (https://github.com/beneater/eeprom-programmer)


## Documentation

This is my updated version of the Ben Eater multiplexed-display. As I'm using a 28C256 EEPROM there is lots of
additional space in the chip of additional display modes. I've implemented:

* Mode 0 - Text
* Mode 1 - Unsigned numbers
* Mode 2 - Signed numbers
* Mode 3 - Hex
* Mode 4 - Reserved
* Mode 5 - Reserved
* Mode 6 - Debug
* Mode 7 - Off

Mode 0 is used to display units to display short strings of predefined text. 
Modes 1 and 2 are the display modes Ben used in his original design
Modes 3 displays numbers as hex codes using the middle two display units
Modes 4 and 5 don't do anything
Mode 6 is similar to mode 0 except the strings are used to help debug the wiring in the display  unit
Mode 7 is used to display nothing except the decimal points (so you know the display is working). 
			
			
My implementation of the 8 bit computer was extended to allow the machine to set it's own display mode. 
The machine powers up in mode 0 with string 0 in the display register, so the the 7 segment displays shows
word 'on'

Mode 7 is used to prevent strange effects when swapping modes.

			
		
## Libraries used

Just the standard Arduino libraries