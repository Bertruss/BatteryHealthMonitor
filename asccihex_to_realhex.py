import sys

if __name__ == "__main__":
    args = sys.argv
    inputfile = args[1]
    outputfile = args[2]
    fd = open(inputfile, 'r')
    fdo = open(outputfile+".hex", "wb")
    for x in fd:
        str_hex = x.split('//')[0].split(',')
        for h in str_hex:
            val = bytes([int(h, 16)])
            fdo.write(val)
    fdo.close()
