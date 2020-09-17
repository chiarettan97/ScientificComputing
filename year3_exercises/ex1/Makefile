:.PRECIOUS

all::  year3.ex3.nearfield_sinusoidal year3.ex3.nearfield year3.ex3.fraunhofer_sinusoidal year3.ex3.fraunhofer year3.ex2.pendulum_b year3.ex2.pendulum year3.ex1.gsl_qags year3.ex1.gsl  year3.ex1


year3.ex3.nearfield_sinusoidal : year3.ex3.nearfield_sinusoidal.o
	g++ -Wall year3.ex3.nearfield_sinusoidal.o -o year3.ex3.nearfield_sinusoidal `pkg-config --libs gsl` -lfftw3

year3.ex3.nearfield : year3.ex3.nearfield.o
	g++ -Wall year3.ex3.nearfield.o -o year3.ex3.nearfield `pkg-config --libs gsl` -lfftw3

year3.ex3.fraunhofer_sinusoidal : year3.ex3.fraunhofer_sinusoidal.o
	g++ -Wall year3.ex3.fraunhofer_sinusoidal.o -o year3.ex3.fraunhofer_sinusoidal `pkg-config --libs gsl` -lfftw3

year3.ex3.fraunhofer : year3.ex3.fraunhofer.o
	g++ -Wall year3.ex3.fraunhofer.o -o year3.ex3.fraunhofer `pkg-config --libs gsl` -lfftw3

year3.ex2.pendulum_b : year3.ex2.pendulum_b.o
	g++ -Wall year3.ex2.pendulum_b.o -o year3.ex2.pendulum_b `pkg-config --libs gsl`

year3.ex2.pendulum : year3.ex2.pendulum.o
	g++ -Wall year3.ex2.pendulum.o -o year3.ex2.pendulum `pkg-config --libs gsl`

year3.ex1.gsl_qags : year3.ex1.gsl_qags.o
	g++ -Wall year3.ex1.gsl_qags.o -o year3.ex1.gsl_qags `pkg-config --libs gsl`

year3.ex1.gsl : year3.ex1.gsl.o
	g++ -Wall year3.ex1.gsl.o -o year3.ex1.gsl `pkg-config --libs gsl`

year3.ex1 : year3.ex1.o
	g++ -Wall year3.ex1.o -o year3.ex1 `pkg-config --libs gsl`
