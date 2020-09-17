// Monte-Carlo Integration of Fresnel Integrals ( Core Task 2 )

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "gsl/gsl_rng.h"
#include "gsl/gsl_monte.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_monte_plain.h"
#include "gsl/gsl_monte_miser.h"
#include "gsl/gsl_monte_vegas.h"

#define N  1000000	// number of calls
#define N_PLOTS	 100	// number of values of u

using namespace std ;

// function to be integrated
static
double
sine_function( double *k, size_t dim, void * params )
{
double sine_u ;
	sine_u = sin( 0.5 * M_PI * k [0] * k [0] ) ;
	return sine_u ;
}

static
double
cosine_function ( double *k, size_t dim, void * params )
{
double cosine_u ;
	cosine_u = cos( 0.5 * M_PI * k [0] * k [0] ) ;
	return cosine_u ;
}

static
void
display ( const char *integral, double result, double error)
{
	cout << "The integral of " << integral << "=" << result << endl ;
	cout << "The error = " << error << endl ;
}

int
main()
{
double sine_result, sine_error, cosine_result, cosine_error ;


gsl_monte_function sine_u = { &sine_function, 1, 0 } ;
gsl_monte_function cosine_u = { &cosine_function , 1, 0 } ;

ofstream outfile ;

// random number generator initialisation
gsl_rng *r = gsl_rng_alloc (gsl_rng_default) ;

	gsl_rng_env_setup () ;

	outfile.open ("monte_carlo_fresnel.dat", ios::trunc) ;

	for ( int i = 1; i < N_PLOTS; i ++ )
	{
	double xl[1] = {0} ;	// set upper and lower limits of integration
	double xu[1] = {i} ;
		{
			gsl_monte_plain_state *s = gsl_monte_plain_alloc (1) ;
			gsl_monte_plain_integrate (&sine_u, xl, xu, 1, N, r, s, &sine_result, &sine_error) ;
			gsl_monte_plain_free (s) ;
			display ( "S(u)", sine_result, sine_error ) ;
		}

		{
			gsl_monte_plain_state *s = gsl_monte_plain_alloc (1) ;
			gsl_monte_plain_integrate (&cosine_u, xl, xu, 1, N, r, s, &cosine_result, &cosine_error) ;
			gsl_monte_plain_free (s) ;
			display ( "C(u)", cosine_result, cosine_error ) ;
		}

		outfile << i << "\t" << sine_result << "\t" << sine_error << "\t" << cosine_result << "\t" << cosine_error << endl ;
	}

	gsl_rng_free (r) ;

	outfile.close () ;


	return 0 ;
}
