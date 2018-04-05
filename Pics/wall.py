#Written by Thomas Peterson
#Used to generate the wall image for the Data.c file

import sys
i = 0
amount = 128
while(i < amount):
    if ((i%10)==0 and (i != 0)):
        print""
    if (i == amount-1):
        sys.stdout.write("0xff")
    else:
        sys.stdout.write("0xff,")
    i+=1
raw_input()
