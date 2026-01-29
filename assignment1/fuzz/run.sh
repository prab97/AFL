#!/bin/bash

sudo docker build -t sshs_asg1_g4 -f Dockerfile .
sudo docker run -v .:/workspace -it sshs_asg1_g4
#sudo afl-cc js.c -o test.o
#sudo afl-fuzz -i in/ -o observation ./test.o

