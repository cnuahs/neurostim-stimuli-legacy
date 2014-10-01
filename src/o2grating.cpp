// o2grating.cpp - static 2nd-order grating stimulus

// 1/10/2014 - Shaun L. Cloherty <s.cloherty@ieee.org>

#include <stdio.h> // NULL

#include "o2grating.h"
#include "neurostim.h"

#include "adams_little_helper.h"

using namespace std;

nsSTIMFACTORY(o2grating);

o2grating::o2grating()
{
	name = "o2grating"; 

//	rcsVersion = ""; // FIXME: add git tag info here...?

	// generate the OpenGL texture
	glGenTextures(1,textureName);
}

o2grating::~o2grating()
{
	for (unsigned int i = 0; i < texPtrs.size(); i++) {
		free(texPtrs[i]);
	}

	// delete the OpenGL texture
	glDeleteTextures(1,textureName);
}

void o2grating::setup()
{
	float elapsedTime;

	// aperture parameters
	stencilBit = getParmInt("stencilBit",1); // (1-8) each texture drawn simultaneously should have a different stencilBit value
	outerDiam = getParm("outerDiam",10.0); // outer diameter for the circular or annular aperture
	innerDiam = getParm("innerDiam",0.0); // inner diameter for the annular aperture

	// parameters defining the carrier grating
	cOrientation = getParm("carrier:orientation",0);
	cSpatialFreq = getParm("carrier:spatialFreq",0);
	cSpatialPhase = getParm("carrier:spatialPhase",0);
	cContrast = getParm("carrier:contrast",0.5);

	// parameters defining the envelope
	eOrientation = getParm("envelope:orientation",0);
	eSpatialFreq = getParm("envelope:spatialFreq",0);
	eSpatialPhase = getParm("envelope:spatialPhase",0);

	meanLum = getParm("meanLum",-1); // -1 = background luminance

	preCompute = getParm("preCompute",true);

	xCIE = getParm("CIE:xCIE",nsGlobal->getDouble("WINDOW:BACKGROUND:XCIE"));  // -1 = monitor white
	yCIE = getParm("CIE:yCIE",nsGlobal->getDouble("WINDOW:BACKGROUND:YCIE"));  // -1 = monitor white

	// get window dimensions in pixels
	float wPix = nsGlobal->getFloat("Window:XPixels");
	float hPix = nsGlobal->getFloat("Window:YPixels");

	// get window dimensions in "units"
	float wUnits = nsGlobal->getFloat("Window:Width");
	float hUnits = nsGlobal->getFloat("Window:Height");

	double aspectRatio1 = round2nDec(wUnits/hUnits, 4);
	double aspectRatio2 = round2nDec(wPix/hPix, 4);

	if (aspectRatio1 != aspectRatio2){
		errorMsg = "Aspect ratio mismatch. Check window width/height and XPIXELS/YPIXELS";
	}

	pixPerUnit = ((wPix/wUnits)+(hPix/hUnits))/2;

	fixPosition(); // round position to nearest pixel

	// set aperture
	vector<double> args;
	args.push_back(stencilBit);

	nsOpenGL("APERTURE","CLEAR",args);

	args.push_back(0); // x
	args.push_back(0); // y

	args.push_back(innerDiam); // innerDiam = 0 produces a circular aperture...?
	args.push_back(outerDiam);
	
	nsOpenGL("APERTURE","ANNULUS",args);		

	// calculate required image size (in pixels)
	imageSize = int(floor(outerDiam*pixPerUnit));
	if (imageSize >= int(floor(hPix))) {
		imageSize = int(floor(hPix))-1;
	}
	// make it odd (ensures it can be centered)
	if ((imageSize % 2) == 0){
		imageSize = imageSize + 1;
	}
	imageHalfWidth = int(floor(float(imageSize)/2));

	// proportion of the texture containing the image
	texProp = 1.0; // by definition...

	if (trial < 0) {
		// note: this code runs only during Neurostim's check of each stimulus
		//       configuration prior to running real trials (indicated by trial = -1,
		//       condition = -1 and block = -1)
		
		// allocate memory for the current texture
		texture = (GLfloat *)malloc(imageSize*imageSize*3*sizeof(GLfloat));

		// calculate the texture
		calcLumVals();
		calcTexture();

		if (preCompute) {
			// store texture pointer
			if (texPtrs.size() < config) {
				errLog << "Extending texPtrs to store " << config << " configurations." << endl;
			
				// FIXME: this assumes that Neurostim tests configurations in
				//        numerical order... which it seems to do.
				texPtrs.push_back(texture);
			}
		} else {
			free(texture); // note: cleanUp() is not called during config testing, so free() texture here
		}

		return;
	}

	// now we're rockin' in the real world... by which I mean that real trials have started (i.e., trial > 0)
	tic();
	errLog << "Showing config " << config << " of " << texPtrs.size() << endl;

	if (preCompute){
		// point to the right texture
		texture = texPtrs[config-1];
	} else {
		// allocate memory for the texture
		texture = (GLfloat *)malloc(imageSize*imageSize*3*sizeof(GLfloat));

		// calculate the texture
		calcLumVals();
		calcTexture();
	}

	// bind the texture... was in calcTexture()
	glBindTexture(GL_TEXTURE_2D,textureName[0]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageSize, imageSize, 0, GL_RGB, GL_FLOAT, texture);          
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	elapsedTime = toc();
	errLog << "setup() took " << elapsedTime << "ms" << endl;

	if (!errorMsg.empty()) {
		warning(errorMsg);
	}
	return;
}

void o2grating::move()
{
	return;
}

void o2grating::draw()
{
	float hw = 0.5*float(imageSize)/float(pixPerUnit); // half width in units?

	// draw the texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,textureName[0]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0,0.0);                  glVertex2f(-hw, -hw);
	glTexCoord2d(0.0,texProp);              glVertex2f(-hw, hw);
	glTexCoord2d(texProp,texProp);          glVertex2f(hw, hw);
	glTexCoord2d(texProp,0.0);              glVertex2f(hw, -hw);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
}

void o2grating::cleanUp()
{
	if (!preCompute){
		free(texture);
	}
}

void o2grating::calcLumVals()
{
	// calculate the luminance value of each pixel in the texture
	double tmpLum, x_prime;
	vector<double> yLum;

	double cOri = deg2rad(cOrientation);
    double cSF = cSpatialFreq/pixPerUnit; // cycles per pixel
	double cSP = deg2rad(cSpatialPhase);

	double eOri = deg2rad(eOrientation);
	double eSF = eSpatialFreq/pixPerUnit; // cycles per pixel
    double eSP = deg2rad(eSpatialPhase);

	double mn = int(meanLum) == -1 ? nsGlobal->getFloat("WINDOW:BACKGROUND:LUMINANCE") : meanLum; // mean luminance, -1 = background

	lumVals.clear(); // FIXME: nasty hack!?
	for (int xPix = -imageHalfWidth; xPix <= imageHalfWidth; xPix++) {
		for (int yPix = -imageHalfWidth; yPix <= imageHalfWidth; yPix++) {

			// rotate carrier
			x_prime = cos(cOri)*xPix + sin(cOri)*yPix;

			// calculate luminance profile of the carrier grating
			tmpLum =  cos(2 * PI * cSF * x_prime + cSP);

			// rotate envelope
			x_prime = cos(eOri)*xPix + sin(eOri)*yPix;

			// apply envelope
			tmpLum = tmpLum * 0.5*(cos(2 * PI * eSF * x_prime + eSP) + 1.0);

            // apply contrast and convert to luminance values
			tmpLum = mn*(1 + cContrast * tmpLum); // from convertToLum()

			// assign luminance profile to the current row
			yLum.push_back(tmpLum);
		}

		// assign the current column to the total matrix
		lumVals.push_back(yLum);

		// clear the y axis vector for next loop
		yLum.clear();
	}
}

void o2grating::calcTexture()
{
	// compute R,G,B value of each pixel in the texture
	unsigned int xInd, yInd;
	double r, g, b;
	bool ok;
        
	for (xInd = 0; xInd < lumVals.size(); xInd++) {
		for (yInd = 0; yInd < lumVals.size(); yInd++) {
			// convert luminance to gun values
			ok =  nsMon().chromLumToRGB(xCIE,yCIE,lumVals[xInd][yInd],r,g,b);
			
			if (!ok) {
				errorMsg = "Error converting lum to RGB: out of mon range?";
			}

			// note: arrays (and vectors) are stored in row-major. Therefore, if
			//
			//       T x[D][H][W];
			//
			//       x[i][j][k] == *(x + i*W*H + j*W + k)
			*(texture + (xInd*3*imageSize) + (yInd*3) + 0) = GLfloat(r);
			*(texture + (xInd*3*imageSize) + (yInd*3) + 1) = GLfloat(g);
			*(texture + (xInd*3*imageSize) + (yInd*3) + 2) = GLfloat(b);
		}
	}
}

// note: this method tweaks the values of X and Y inherited from nsStimulus, rounding
//       (i.e., fixing) them to the nearest pixel value to ensure that texels map onto
//       pixels in a one-to-one fashion
void o2grating::fixPosition()
{
	X = floor(X*pixPerUnit)/pixPerUnit;
	Y = floor(Y*pixPerUnit)/pixPerUnit;
}
