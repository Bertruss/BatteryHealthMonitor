from PIL import Image
import pdb

if __name__ == "__main__":
    #read in hex chars and print the monochrome bitmap representation
    f = open("test_bed.txt")
    line_num = 1
    pixel_resolve = (lambda val: 0x01 if int(val) else 0x00)
    pixel_data = []
    for x in f:
        character_string = x.split(',')
        character_string = character_string
        character_string.reverse()
        for val in character_string:
            val = int(val,16)
            lit = format(val, "08b")
            pixel_data.append([pixel_resolve(p) for p in lit])
        ii = len(pixel_data)
        jj = len(pixel_data[0])
        img = Image.new( '1', (5,8), "white") # Create a new black image
        pixels = img.load() # Create the pixel map
        for i in range(len(pixel_data)):  # For every pixel:
            for j in range(len(pixel_data[0])): 
                pixels[i,j] = pixel_data[ii-i-1][j]  # Set the colour accordingly
        print(line_num)
        img.save("line_num"+str(line_num)+".bmp" ,format="bmp")
        line_num += 1
        pixel_data = []
