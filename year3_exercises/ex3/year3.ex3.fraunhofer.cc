#include <iostream>
#include <fstream>
#include <vector>
#include "math.h"
#include "fftw3.h"

#define N  512	// size of FT - FFT is fastest when this is a power of 2. Need N > 2 * EXTENT / WIDTH
#define LAMBDA  0.5		// distances measured in microns
#define EXTENT  5000
#define WIDTH  100
#define SCREEN_DISTANCE  1e6  

using namespace std ;

int
main (int argc, char **argv)
{
/*
fftw_complex *input, *output ;

	input = (fftw_complex *) fftw_malloc ( sizeof (fftw_complex)*N ) ;		// allocate memory - cast fftw_malloc to fftw_complex pointer
	output = (fftw_complex *) fftw_malloc ( sizeof (fftw_complex)*N ) ;
*/


std::vector <double> inp ( 2 * N , 0 ) ;
std::vector <double> out ( 2 * N , 0 ) ;
fftw_plan plan ;		// write plan before initialising input

double x_step = EXTENT / N ;
double x [N] ;		// x coordinates of aperture
double q [N] ;
double y [N] ;		// y coordinates of diffraction pattern
double intensity [N] ;
double phase [N] ;

ofstream outfile ;
ofstream paramfile ;

	paramfile.open ( "fraunhofer_slit.params", ios::trunc ) ;
	paramfile << LAMBDA << '|' << SCREEN_DISTANCE << '|' << WIDTH << endl ;
	paramfile.close () ;	

	
	
	// initalise array of x values (assume N is even so x[N/2] = 0 is always a member of the array)
	// initialise real values of input to be non-zero
	// transmission function for slit of width d and aperture with total extent EXTENT, sampled at N points
	for ( int i = 0 ; i < N ; i ++ )
	{
//		cout << "i = " << i <<endl ;

		x [i]  = ((double) i - ( 0.5 * N ) ) * x_step ;
	
		if ( (double) i <= ( 0.5 * N ) )
		{
			q [i] = ((double) i / ( (double) N * x_step )) ;
		}

		else 
		{
			q [i] = ( ((((double) i / (double) N) - 1.0)) / x_step ) ;
		}

//		cout << "x = " << x [i] << endl ;
//		cout << "q = " << q [i] << endl ;

		y [i] = q [i] * LAMBDA / SCREEN_DISTANCE ;

		// transmission function for slit
		if ( fabs ( x [i] ) <= ( 0.5 * WIDTH ) )
		{
			inp [ 2 * i ] = 1 ;
		}
	}

	fftw_complex *input = ( fftw_complex * ) &inp [0] ;	
	fftw_complex *output = ( fftw_complex * ) &out [0] ;


	plan = fftw_plan_dft_1d ( N , input, output, FFTW_FORWARD, FFTW_ESTIMATE ) ;

	fftw_execute (plan) ;

	outfile.open( "fraunhofer_slit.dat" , ios::trunc ) ;

/*
	for ( int i = 0 ; i < ( 2 * N ) ; i ++ )
	{
		cout << "A (x) = " << input [ i ] << "FT (q) = " << output [ i ] << endl ;
		outfile << "A(x) = " << input [ i ] << "\t" << "FT (q) = " << output [ i ] << endl ;
	}
*/
	for ( int i = 0 ; i < N ; i ++ )
	{
		intensity [i] = pow ( (out [ 2 * i ] * EXTENT/N) , 2 ) + pow ( (out [ 2 * i + 1 ] * EXTENT/N), 2 ) ;
		phase [i] = atan ( out [ 2 * i + 1 ] / out [ 2 * i ] ) ;
	
//		cout << i << endl ;
//		cout << "A (x) = " << inp [ 2 * i ] << " " << inp [ 2 * i + 1 ] << "    FT (q) = " << out [ 2 * i ] << " " << out [ 2 * i + 1 ] << endl ;
		outfile << q [ i ] << "\t" << y [ i ] << "\t" << out [ 2 * i ] << "\t" << out [ 2 * i + 1 ] << "\t" << intensity [i] << "\t" << phase [i] << endl ;
	}

	outfile.close() ;

	
	// deallocate memory
	fftw_destroy_plan (plan) ;
/*
	fftw_free (input) ;
	fftw_free (output) ;
*/

	return 0 ;
}
