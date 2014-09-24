#!/usr/bin/python

import os
import sys

def main(inputFile, outputFile):

    errorFound = False
    inLines = []
    outLines = []
    
    with open(inputFile) as f:
        inLines = f.readlines()

    with open(outputFile) as f:
        outLines = f.readlines()
      
    for i in range(0, len(inLines)-1):
        if inLines[i] != outLines[i]:
            errorFound = True;
            print "Lines %d  not equal" % i
#            print "file 1: %s" % inLines[i]
#            print "file 2: %s" % outLines[i]

    if errorFound == False:
        print "Files are identical"

if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(sys.argv[1], sys.argv[2])
    else:
        print "Wrong number of arguments"
