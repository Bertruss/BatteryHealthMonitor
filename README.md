## Battery Health Monitor

ATtiny45/85 based dedicated battery health monitor for embedded projects, with small status readout

# Setup Notes:
- lfuse set to 0xE2 to remove the DIV8 and set the clock to full 8mhz

*example: programming using the USBtinyISP via avrdude*
```
avrdude -c usbtiny -p t45 -U hfuse:w:0xE2:m
```
- The font data was intended to be written to eeprom on the chip to save flash, 
not knowing what the total compiled footprint of the code would be beforehand, I assumed I would need to be covetous of the limited 4k flash, and yet as development went on that seemed less and less necessary ¯\\_(ツ)_/¯. 