#include "adams_little_helper.h"
#include "neurostim.h"
using namespace std;

//double ticTime = -1;

int fix(double x)
{
	//rounds the elements of X to the nearest integers towards zero.
	int i;

	if (x <= 0){
		i = int(ceil(x));
	}
	else{
		i = int(floor(x));
	}

	return i;
}



double deg2rad(double deg){
	double rad = deg/(180/double(PI));
	return rad;
}


double rad2deg(double rad){
	double deg = rad*(180/double(PI));
	return deg;
}

double round2nDec(const double &number, const int num_digits)
{
	// If num_digits is 0, then number is rounded to the nearest integer.
	// Examples
	//        ROUND(2.15, 1)        equals 2.2
	//        ROUND(2.149, 1)        equals 2.1
	//        ROUND(-1.475, 2)    equals -1.48   double doComplete5i, doComplete5(number * powf(10.0f, (double) (num_digits + 1)));
    
    float doComplete5i, doComplete5(number * powf(10.0f, (double) (num_digits + 1)));

	if(number < 0.0f)
        doComplete5 -= 5.0f;
    else
        doComplete5 += 5.0f;
    
    doComplete5 /= 10.0f;
    modff(doComplete5, &doComplete5i);
    
    return doComplete5i / powf(10.0f, (double) num_digits);
}

//void tic()
//{
//	tictime = nstime();
//}

//double toc(bool echo)
//{
//	double elapsedTime = double(nsTime() - ticTime);
//	
//	if (ticTime != -1){
//		if (echo){
//			cout << "Elapsed time = " << elapsedTime << "ms" << endl;
//		}
//	}
//	else{
//		cout << "Error: toc() called without a prior call to tic()" << endl;
//	}
//
//	return elapsedTime;
//}

void pol2cart(double theta, double r, double* x, double* y)
{
	*x = r*cos(theta);
	*y = r*sin(theta);
}

void pold2cart(double theta, double r, double* x, double* y)
{
	*x = r*cos(deg2rad(theta));
	*y = r*sin(deg2rad(theta));
}

void cart2pol(double x, double y, double* theta, double* r)
{
	*theta = atan2(y,x);
	*r = sqrt(x*x + y*y);
}

void cart2pold(double x, double y, double* theta, double* r)
{
	*theta = rad2deg(atan2(y,x));
	*r = sqrt(x*x + y*y);
}


void waitMS(double waitTime)
{
	tic();

	double g = toc();
	while (g < waitTime){
		g = toc();
	}
	return;
}



double frames2ms(int frames, double frameRateHz)
{
	double msOut = frames * (1000/frameRateHz);
	
	return msOut;
}


int ms2frames(double ms, double frameRateHz)
{
	float frIn = ms/(1000/frameRateHz);
	
	int frOut =	int(round2nDec(frIn,0));

	return frOut;
}

void rotateXY(double x, double y, double thetaRad, double* xr, double* yr)
{

	
	*xr = cos(thetaRad)*x - sin(thetaRad)*y;
	*yr = sin(thetaRad)*x + cos(thetaRad)*y;


}