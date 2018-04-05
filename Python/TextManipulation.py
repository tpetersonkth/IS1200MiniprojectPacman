import sys
#Used to format the instruction text data (this file is not sent to the mips processor)
while(True):
    text = raw_input("Text: ")
    text = text.split(" ")
    output = [""]
    index = 0
    for word in text:
        if (len(output)-1) < index:
            print(str(len(output)-1)+" < "+ str(index))
            output.append(word)
        elif len(output[index])+1+len(word) <= 21:
             output[index] = output[index]+" "+word
        else:
            output.append(word)
            index += 1

    f = open("output.txt","w")
    count = 0
    for sentence in output:
        if count <= 0:
            sys.stdout.write("{")
        if count != 3 and sentence != output[-1]:
            sys.stdout.write("\""+sentence+"\",")
        else:
            sys.stdout.write("\""+sentence+"\"")
            if sentence == output[-1]:
                sys.stdout.write("}")
        count += 1
        if (count >= 4):
            sys.stdout.write("},\n")
            count = 0
