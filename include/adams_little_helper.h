#if !defined(ADAMS_LITTLE_HELPER_H)
#define ADAMS_LITTLE_HELPER_H

 
int fix(double);  //rounds the elements of X to the nearest integers towards zero.
double deg2rad(double);
double rad2deg(double);
void pol2cart(double theta, double r, double* x, double* y);
void pold2cart(double theta, double r, double* x, double* y);
void cart2pol(double x, double y, double* theta, double* r);
void cart2pold(double x, double y, double* theta, double* r);
void rotateXY(double x, double y, double thetaRad, double* xr, double* yr);
double round2nDec(const double &number, const int num_digits);
void waitMS(double);

int ms2frames(double ms, double frameRateHz);
double frames2ms(int frames, double frameRateHz);


#endif