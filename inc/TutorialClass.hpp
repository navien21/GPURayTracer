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
#include "RayTraceImageData.h"
#include "jsonParameterReader.h"


using namespace optix;

//-----------------------------------------------------------------------------
// 
// Whitted Scene
//
//-----------------------------------------------------------------------------

class Tutorial : public SampleScene
{
public:
  Tutorial(int tutnum, const std::string& texture_path, const RayTraceImageData inImage)
    : SampleScene(), m_tutnum(tutnum), m_width(1080u), m_height(720u), texture_path( texture_path ), imageData(inImage)
  {}
  
  // From SampleScene
  void   initScene( InitialCameraData& camera_data );
  void   trace( const RayGenCameraData& camera_data );
  void   doResize( unsigned int width, unsigned int height );
  void   setDimensions( const unsigned int w, const unsigned int h ) { m_width = w; m_height = h; }
  Buffer getOutputBuffer();

private:
  std::string texpath( const std::string& base );
  void createGeometry();

  unsigned int m_tutnum;
  unsigned int m_width;
  unsigned int m_height;
  std::string   texture_path;
  std::string  m_ptx_path;
  RayTraceImageData imageData;
};

