// o2grating.h - static 2nd-order grating stimulus

// 20/9/2014 - Shaun L. Cloherty <s.cloherty@ieee.org>

#if !defined(_o2grating_H)
#define _o2grating_H

// include the neurostim header
#include "neurostim.h"

//#include <string>

// new stimulus class is derived from the nsStimulus class
class o2grating:
  public nsStimulus
	{
	public:
		// overide methods from the nsStimulus class
		void setup(); // called before the start of each trial
		void move();  // called before every frame. Implement animation or other
		              // cpu intensive opertions in this function.
		void draw();  // called before every frame but after move(). This function
		              // should contain the OpenGL drawing code

		// overide constructor and destructor
		o2grating();
		virtual ~o2grating();

		// keyboard handler
//		void keyboard(const unsigned char key, const int x, const int y);

//		void cleanUp(); // called after the end of each trial?

		void fixPosition();

	protected:
		// member variables that define the stimulus
		nsDouble xCIE, yCIE;
		nsDouble meanLum;

		// aperture parameters
		int stencilBit; // each stimulus on the screen at any one time needs a different stencil bit!
		nsDouble outerDiam, innerDiam;

		double pixPerUnit;
		
		int imageSize;
		int imageHalfWidth;

		// parameters defining the carrier grating
		nsDouble cSpatialFreq;
		nsDouble cOrientation;
		nsDouble cSpatialPhase;

		nsDouble cContrast;

		// parameters defining the envelope
		nsDouble eSpatialFreq;
		nsDouble eOrientation;
		nsDouble eSpatialPhase;

		std::vector<std::vector<double>> lumVals;

		void calcLumVals(); // calculates luminance values in lumVals

//		void convertToLum(); // DEPRECATED

		// fancy OpenGL stuff...?
		GLdouble texProp;
		GLuint textureName[1];
		static const int NRTEXELS = 1024;
//		GLfloat texture[NRTEXELS][NRTEXELS][3];
		GLfloat *texture;

		std::vector<GLfloat *> texPtrs; // pointers to pre-computed "textures"

		std::string errorMsg;
		void calcTexture(); // calculates R,G,B values in texture
	};

#endif
