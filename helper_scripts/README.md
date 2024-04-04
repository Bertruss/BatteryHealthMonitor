## Helper scripts

I used these scripts to prune and inspect the 5x8 pixel (so 5 byte) font style.


The data was copied from someone elses incredibly helpful repo of font styles and sizes. I really didn't want to 
have to whip up my own bitmap font...


Anyway, in the font folder, you can feed the fnt.txt file into the character_set_viewer.py (dependent on PIL) 
and generate separate tiny bitmap images of the characters therein. I used this to review and cut down the originally 256
character font down to 51 characters to fit in the eeprom of the ATtiny45. 51 is tight, but also, there are a lot graphics characters in these fonts that we don't have a specific need for.


credit to user basti79 for the original font: https://github.com/basti79/LCD-fonts/blob/master/5x8_vertikal_LSB_1.asm

Anyway, once that was done, I used the asccihex_to_realhex.py to convert the written out human readable hex to a raw
binary file, exactly 256 bytes in length. output file is automatically given the extension '.hex'.

```
    python asccihex_to_realhex.py <text font file> <output filename>
```


Once that is done, you can burn the .hex file with avrdude to the mcu's eeprom. 
```
    avrdude -c usbtiny -p t45 -U eeprom:w:fnt.hex:r
```

