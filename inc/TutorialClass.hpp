/* Includes the Tutorial class delaration.  This class was derived from the
   Tutorial sample given in the NVDIA Optix SDK.
   */


#pragma once

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_namespace.h>
#include <iostream>
#include <GLUTDisplay.h>
#include <ImageLoader.h>
#include "GPUManager.hpp"
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <math.h>

#include <Model.hpp>

using namespace optix;

class Tutorial : public SampleScene
{
	public:
		Tutorial(const grt::Model & _model)
			: SampleScene()
			  , m_width(1080u)
			  , m_height(720u)
			  , model(_model)
	{}

		// From SampleScene
		void initScene( InitialCameraData& camera_data );
		void trace( const RayGenCameraData& camera_data );
		void doResize( unsigned int width, unsigned int height );
		void setDimensions( const unsigned int w, const unsigned int h ) { m_width = w; m_height = h; }
		Buffer getOutputBuffer();

	private:
		grt::Model model;
		std::string texpath( const std::string& base );
		void createGeometry();

		unsigned int m_width;
		unsigned int m_height;
		std::string texture_path;
		std::string m_ptx_path;
};

