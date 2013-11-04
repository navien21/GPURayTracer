//#pragma once

#include <optix.h>
#include <optix_math.h>
#include <optixu/optixu_vector_types.h>
#include <device_functions.h>

typedef struct struct_BasicLight
{
#if defined(__cplusplus)
  typedef optix::float3 float3;
#endif
  float3 pos;
  float3 color;
  int    casts_shadow; 
  int    padding;      // make this structure 32 bytes -- powers of two are your friend!
} BasicLight;

struct TriangleLight
{
#if defined(__cplusplus)
  typedef optix::float3 float3;
#endif
  float3 v1, v2, v3;
  float3 normal;
  float3 emission;
};


#ifndef GRT_GPUMANAGER_HPP
#define GRT_GPUMANAGER_HPP

namespace grt {

    // Forward declarations
    class Scene;
    class Image;

    class GPUManager
    {
        public:
            void upload(const Scene &);
            void render();
            void download(Image &);
    };

}; // grt namespace

#endif//GRT_GPUMANAGER_HPP

// tutorial.h content below... yet to get working
/*

using namespace optix;

#define FLT_MAX         1e30;

static __device__ __inline__ float3 exp( const float3& x )
{
  return make_float3(exp(x.x), exp(x.y), exp(x.z));
}

static __device__ __inline__ float step( float min, float value )
{
  return value<min?0:1;
}

static __device__ __inline__ float3 mix( float3 a, float3 b, float x )
{
  return a*(1-x) + b*x;
}

static __device__ __inline__ float3 schlick( float nDi, const float3& rgb )
{
  float r = fresnel_schlick(nDi, 5, rgb.x, 1);
  float g = fresnel_schlick(nDi, 5, rgb.y, 1);
  float b = fresnel_schlick(nDi, 5, rgb.z, 1);
  return make_float3(r, g, b);
}

static __device__ __inline__ uchar4 make_color(const float3& c)
{
    return make_uchar4( static_cast<unsigned char>(__saturatef(c.z)*255.99f),  // B 
                        static_cast<unsigned char>(__saturatef(c.y)*255.99f),  // G 
                        static_cast<unsigned char>(__saturatef(c.x)*255.99f),  // R 
                        255u);                                                 // A 
}

struct PerRayData_radiance
{
  float3 result;
  float  importance;
  int depth;
};

struct PerRayData_shadow
{
  float3 attenuation;
};

*/