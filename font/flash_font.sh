#!/bin/bash
# programmer: usbtiny
# part: Attiny45
avrdude -c usbtiny -p t45 -U eeprom:w:fnt.hex:r