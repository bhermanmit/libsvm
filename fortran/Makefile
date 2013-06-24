# This Makefile is used under Linux

CXX = g++
CFLAGS = -Wall -Wconversion -O3 -fPIC -I$(MATLABDIR)/extern/include -I..

all:	fortran-lib

fortran-lib:	../svm.o fortran
	ar rcs libsvm.a ../svm.o svm_fortran.o

fortran:	svm_fortran.o
	$(CXX) $(CFLAGS) -c svm_fortran.cpp

../svm.o: ../svm.cpp ../svm.h
	make -C .. svm.o

clean:
	rm -f *~ *.o *.mex* *.obj ../svm.o
