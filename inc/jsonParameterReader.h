
#ifndef RAY_TRACE_PARAMETER_READER_H
#define RAY_TRACE_PARAMETER_READER_H

#include "RayTraceImageData.h"
#include <iostream>
#include <cstring>

void LoadRayTraceImageFromJsonFile(std::string fileName, RayTraceImageData &image);

#endif