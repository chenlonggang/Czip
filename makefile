CC=g++
CFLAGS= -O3 -Wall -std=c++11
my_csa:div main.o csa.a 
	g++ main.o csa.a -o my_csa -lpthread

csa.a: CSA.o CSA_chunk.o Phi.o savekit.o loadkit.o divsufsort.o sssort.o trsort.o utils.o
	ar rc csa.a CSA.o CSA_chunk.o Phi.o savekit.o loadkit.o divsufsort.o sssort.o trsort.o utils.o
%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
main.o:main.cpp  CSA.h
	g++ -c main.cpp 
clean:
	rm *.a  *.o
div:
	make -C ./divsufsort/ ;cp divsufsort/libdivsufsort.a .;ar x libdivsufsort.a;rm libdivsufsort.a 

