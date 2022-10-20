FLAGS=-lglfw -ldl -lm -g -gdwarf-2
all: app

app: main.o glad.o
	g++ -o app main.o glad.o $(FLAGS)

main.o: main.cpp glad.h
	g++ -c main.cpp $(FLAGS)

glad.o: glad.c
	g++ -c glad.c


