
/*
 * Copyright (c) 2008 - 2009 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and proprietary
 * rights in and to this software, related documentation and any modifications thereto.
 * Any use, reproduction, disclosure or distribution of this software and related
 * documentation without an express license agreement from NVIDIA Corporation is strictly
 * prohibited.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS*
 * AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY
 * SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR
 * INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES
 */

#include <optix_world.h>
#include <cfloat>

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal,   attribute shading_normal, ); 
rtDeclareVariable(optix::Ray, ray,          rtCurrentRay, );

using namespace optix;

//
// Bounding box program for programmable convex hull primitive
//
rtDeclareVariable(float3, chull_bbmin, , );
rtDeclareVariable(float3, chull_bbmax, , );
RT_PROGRAM void chull_bounds (int primIdx, float result[6])
{
  optix::Aabb* aabb = (optix::Aabb*)result;
  aabb->m_min = chull_bbmin;
  aabb->m_max = chull_bbmax;
}

//
// Intersection program for programmable convex hull primitive
//
rtBuffer<float4> planes;
RT_PROGRAM void chull_intersect(int primIdx)
{
  int n = planes.size();
  float t0 = -FLT_MAX;
  float t1 = FLT_MAX;
  float3 t0_normal = make_float3(0);
  float3 t1_normal = make_float3(0);
  for(int i = 0; i < n && t0 < t1; ++i ) {
    float4 plane = planes[i];
    float3 n = make_float3(plane);
    float  d = plane.w;

    float denom = dot(n, ray.direction);
    float t = -(d + dot(n, ray.origin))/denom;
    if( denom < 0){
      // enter
      if(t > t0){
        t0 = t;
        t0_normal = n;
      }
    } else {
      //exit
      if(t < t1){
        t1 = t;
        t1_normal = n;
      }
    }
  }

  if(t0 > t1)
    return;

  if(rtPotentialIntersection( t0 )){
    shading_normal = geometric_normal = t0_normal;
    rtReportIntersection(0);
  } else if(rtPotentialIntersection( t1 )){
    shading_normal = geometric_normal = t1_normal;
    rtReportIntersection(0);
  }
}
