#include <iostream>
#include <vector>
#include "math.h"
#include "fftw3.h"

#define N  8	// size of FT - FFT is fastest when this is a power of 2
#define LAMBDA  0.5		// distances measured in microns
#define EXTENT  5000
#define WIDTH  100
#define SCREEN_DISTANCE  1e6  

using namespace std ;

int
main ()
{
fftw_complex *input, *output ;

	input = (fftw_complex *) fftw_malloc ( sizeof (fftw_complex)*N ) ;		// allocate memory - cast fftw_malloc to fftw_complex pointer
	output = (fftw_complex *) fftw_malloc ( sizeof (fftw_complex)*N ) ;


/*
std::vector <double> inp ( 2 * N , 0 ) ;
std::vector <double> out ( 2 * N , 0 ) ;
*/

fftw_plan plan ;		// write plan before initialising input

double x_step = EXTENT / N ;
double x [N] ;
	
/*	
	// initalise array of x values (assume N is even so x[N/2] = 0 is always a member of the array)
	// initialise real values of input to be non-zero
	// transmission function for slit of width d and aperture with total extent EXTENT, sampled at N points
	for ( double d = 0 ; d < N ; i ++ )
	{
		x [i]  = ( i - ( 0.5 * N ) ) * x_step ;
		cout << "x = " << x [i] << endl ;

		if ( fabs ( x [i] ) <= ( 0.5 * WIDTH ) ) ;
			input [ i ] = 1 ;
	}	
*/
/*
	fftw_complex *input = ( fftw_complex * ) &inp [0] ;	
	fftw_complex *output = ( fftw_complex * ) &out [0] ;
*/

	plan = fftw_plan_dft_1d ( N , input, output, FFTW_FORWARD, FFTW_ESTIMATE ) ;

	fftw_execute (plan) ;

	for ( int i = 0 ; i < ( 2 * N ) ; i ++ )
	{
		cout << "A (x) = " << input [ i ] << "FT (q) = " << output [ i ] << endl ;
	}	
	
	// deallocate memory
	fftw_destroy_plan (plan) ;
	fftw_free (input) ;
	fftw_free (output) ;

	return 0 ;
}
