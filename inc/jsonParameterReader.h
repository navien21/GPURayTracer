/* LoadRayTraceImageFromJsonFile function declaration.  
  This function loads a RayTraceImageData object from a JSON file */

#pragma once


#include "RayTraceImageData.h"
#include <iostream>
#include <cstring>

void LoadRayTraceImageFromJsonFile(std::string fileName, RayTraceImageData &image);
