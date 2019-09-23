#Makefile for assignment 2
#Jonathan Soszka
CC = g++ -std=c++11 -Wall -Wextra

all: sim.exe

sim.exe: sim.o
	$(CC) -o sim.exe sim.o

sim.o: sim.cpp
	$(CC) -I. -c sim.cpp
