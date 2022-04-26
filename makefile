main:shell.o function.o function.h
	g++ shell.o function.o function.h -o main
shell.o:shell.cpp
	g++ -c shell.cpp -o shell.o
function.o:function.cpp
	g++ -c function.cpp -o function.o

clean:
	rm *.o
	rm main
