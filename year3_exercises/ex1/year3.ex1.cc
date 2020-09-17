// Monte Carlo Integration from scratch ( Core Task 1 )

#include <math.h>
#include <iostream>
#include <fstream>
#include "gsl/gsl_rng.h"


// N is the number of samples randomly distributed throughout the volume V

#define _USE_MATH_DEFINES
#define DIMENSIONS 8
#define N_INTEGRAL 25		// number of values of the integral used for each N to find the average and error in the integral
#define M_PI_8   (M_PI / 8)
#define N_PLOTS 20		// 2 to the power of this gives N
#define MAX_N_PLOTS 32

using namespace std ;

static
int
generate(gsl_rng *rng, double *values, int n_values)
{

	for (int i = 0; i < n_values ; i ++ )
	{
		values [i] = gsl_rng_uniform (rng) * M_PI_8 ;
		//cout << "Generated value " << i << " : " << values [i] << endl ;
	}

	return 0 ;
}

static
double
sine_n (double *values, int n_values)
{
double sine_value ;
double summa = 0 ;

	for ( int i = 0; i < n_values; i++ )
	{
		summa += values [i] ;
	}

	sine_value = sin (summa) ;

//	cout << "Summa = " << summa << " Sine = " << sine_value << endl ;

	return sine_value ;
}




static
double
calc_integral (gsl_rng * rng, int n_iter, double *theoretical_error)
{
double summa_sine = 0.0 ;		// need to initialise summa_sine outside the loop
double summa_sine_squared = 0.0 ;
double average_sine ;
double average_sine_squared ;

	for ( int i = 0; i < n_iter; i++ )		// for each of the N points, calculate the value of sin
	{
	double values [DIMENSIONS] ;
	double sine_value ;

		generate (rng, values, DIMENSIONS) ;		// this generates N coordinates for each point, stored in values[]

/*
		for (int i = 0; i < DIMENSIONS ; i ++ )
		{
			cout << values [i] << endl ;
		}
*/

		sine_value = sine_n (values, DIMENSIONS) ;		// calculates sin as a function of the N coordinates
		//cout << "sine_value = " << sine_value << endl ;
		summa_sine += sine_value ;
		summa_sine_squared += sine_value * sine_value ;
	}

	average_sine = summa_sine / n_iter ;
	average_sine_squared = summa_sine_squared / n_iter ;
	

//	cout << "Sum of sine values = " << summa_sine << endl ;
//	cout << "The average sine value = " << average_sine << endl ;
//	cout << "The average sine-squared value = " << average_sine_squared << endl ;

double volume , integral;

	volume = pow ( M_PI_8 , DIMENSIONS ) ;
	integral = pow ( 10 , 6 ) * volume * average_sine ;
	*theoretical_error = (volume * sqrt(( average_sine_squared - pow(average_sine, 2.0) ) / n_iter)) ;

	cout << "Theoretical error : " << (*theoretical_error) << endl ;

	return integral ;
}


static
double
calc_integral_error (gsl_rng * rng, int n_iter, double * theoretical_error)
{
double summa = 0.0 ;
double variance = 0.0 ;
double integral_values[N_INTEGRAL] ;
double average_integral ;
double error_integral ;
double theoretical_errors [N_INTEGRAL] ;
double summa_theoretical_error = 0.0 ;
double average_theoretical_error ;

	cout << "n_iter = " << n_iter << endl ;

	for ( int i = 0 ; i < N_INTEGRAL ; i++ )
	{
		integral_values [i] = calc_integral (rng, n_iter, &theoretical_errors [i]) ;
//		cout << "The integral = " << i << " : " << integral_values[i] << endl ;
		summa += integral_values[i] ;
		summa_theoretical_error += theoretical_errors[i] ;
	} 

	average_integral = summa / N_INTEGRAL ;
	average_theoretical_error = summa_theoretical_error / N_INTEGRAL ;
	cout << "The average integral = " << average_integral << endl ;
	cout << "The average theoretical error = " << average_theoretical_error << endl ;

	for ( int i = 0 ; i < N_INTEGRAL ; i++ )
	{
	double difference[N_INTEGRAL] ;
	double squared_difference[N_INTEGRAL] ;
		difference[i] = integral_values[i] - average_integral ;
//		cout << "The difference = " << difference[i] << endl ;

		squared_difference[i] = difference[i] * difference[i] ;
//		cout<< "The squared difference = " << squared_difference[i] << endl ;

		variance += squared_difference[i] ;
	}

	error_integral = sqrt ( variance / N_INTEGRAL ) ;
	cout << "The error in the integral = " << error_integral << endl ;

	*theoretical_error = average_theoretical_error ;

	return error_integral ;
}


int
main (int argc, char **argv)
{
int n_plots = N_PLOTS ;
gsl_rng *rng = gsl_rng_alloc ( gsl_rng_default ) ;    // could have used gsl_rng_taus (the Tausworthe generator)
double errors [MAX_N_PLOTS] ;
double theoretical_errors [MAX_N_PLOTS] ;

	if ( argc > 1 )
	{
	int n = atoi (argv [1]) ;

		if ( n <= 0 || n > MAX_N_PLOTS )
		{
			cerr << "no. of plots is not valid. Continuing with " << n_plots << " plots" << endl ;
		}
		else
		{
			n_plots = n ;
		}
	}

	cout << n_plots << " plots" << endl ;

	gsl_rng_set ( rng, time(NULL) ) ;		// generate the same seed for all N points since time is not fast enough to generate a new seed every time the loop repeats

	for ( int i = 0; i < n_plots; i++ )
	{
		errors [i] = calc_integral_error (rng, (int) pow (2.0, i+1), &theoretical_errors [i]) ;
	}

	for ( int i = 0; i < n_plots; i++ )
	{
		cout << "Error " << i << " : " << errors [i] << endl ;
	}

	gsl_rng_free( rng ) ;

	{
	ofstream outfile ;
		outfile.open ("monte_carlo_sine.error_analysis.dat", ios::trunc) ;
		for ( int i = 0; i < n_plots; i++ )
		{
			outfile << (int) pow (2.0, i+1) << '\t' << errors [i] << endl ;
		}
		outfile.close () ;

		outfile.open ("monte_carlo_sine.theoretical_error_analysis.dat", ios::trunc) ;
		for ( int i = 0; i < n_plots; i++ )
		{
			outfile << theoretical_errors [i] << '\t' << errors [i] << endl ;
		}
		outfile.close () ;
	}

	return 0 ;
}
