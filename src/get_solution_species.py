#!/usr/bin/python

import sys

f = open(sys.argv[1], 'r')
line = f.readline()
if line[:10] != "Infeasible":
    line = f.readline()
    while line != "":
        split = line.strip().split()
        varName = split[1]
        if len(varName) > 2:
            if varName[0:2] == "s/" and split[2] == "1":
                print varName[2:]
        line = f.readline()
