
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

#include "tutorial.h"
//#include "GPUManager.hpp"

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal,   attribute shading_normal, ); 

rtDeclareVariable(PerRayData_radiance, prd_radiance, rtPayload, );
rtDeclareVariable(PerRayData_shadow,   prd_shadow,   rtPayload, );

rtDeclareVariable(optix::Ray, ray,          rtCurrentRay, );
rtDeclareVariable(float,      t_hit,        rtIntersectionDistance, );
rtDeclareVariable(uint2,      launch_index, rtLaunchIndex, );

rtDeclareVariable(unsigned int, radiance_ray_type, , );
rtDeclareVariable(unsigned int, shadow_ray_type , , );
rtDeclareVariable(float,        scene_epsilon, , );
rtDeclareVariable(rtObject,     top_object, , );


//
// Pinhole camera implementation
//
rtDeclareVariable(float3,        eye, , );
rtDeclareVariable(float3,        U, , );
rtDeclareVariable(float3,        V, , );
rtDeclareVariable(float3,        W, , );
rtDeclareVariable(float3,        bad_color, , );
rtBuffer<uchar4, 2>              output_buffer;

RT_PROGRAM void pinhole_camera()
{
  size_t2 screen = output_buffer.size();

  float2 d = make_float2(launch_index) / make_float2(screen) * 2.f - 1.f;
  float3 ray_origin = eye;
  float3 ray_direction = normalize(d.x*U + d.y*V + W);

  optix::Ray ray(ray_origin, ray_direction, radiance_ray_type, scene_epsilon );

  PerRayData_radiance prd;
  prd.importance = 1.f;
  prd.depth = 0;

  rtTrace(top_object, ray, prd);

  output_buffer[launch_index] = make_color( prd.result );
}


//
// Returns solid color for miss rays
//
rtDeclareVariable(float3, bg_color, , );
RT_PROGRAM void miss()
{
  prd_radiance.result = bg_color;
}
  

//
// (NEW)
// Terminates and fully attenuates ray after any hit
//
RT_PROGRAM void any_hit_shadow()
{
  // this material is opaque, so it fully attenuates all shadow rays
  prd_shadow.attenuation = make_float3(0);

  rtTerminateRay();
}
  

//
// (UPDATED)
// Phong surface shading with shadows 
//
rtDeclareVariable(float3,   Ka, , ); 
rtDeclareVariable(float3,   Ks, , ); 
rtDeclareVariable(float,    phong_exp, , );
rtDeclareVariable(float3,   Kd, , ); 
rtDeclareVariable(float3,   ambient_light_color, , );
rtBuffer<BasicLight>        lights;
rtDeclareVariable(rtObject, top_shadower, , );

RT_PROGRAM void closest_hit_radiance3()
{
  float3 world_geo_normal   = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
  float3 world_shade_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
  float3 ffnormal     = faceforward( world_shade_normal, -ray.direction, world_geo_normal );
  float3 color = Ka * ambient_light_color;

  float3 hit_point = ray.origin + t_hit * ray.direction;

  for(int i = 0; i < lights.size(); ++i) {
    BasicLight light = lights[i];
    float3 L = normalize(light.pos - hit_point);
    float nDl = dot( ffnormal, L);

    if( nDl > 0.0f ){
      // cast shadow ray
      PerRayData_shadow shadow_prd;
      shadow_prd.attenuation = make_float3(1.0f);
      float Ldist = length(light.pos - hit_point);
      optix::Ray shadow_ray( hit_point, L, shadow_ray_type, scene_epsilon, Ldist );
      rtTrace(top_shadower, shadow_ray, shadow_prd);
      float3 light_attenuation = shadow_prd.attenuation;

      if( fmaxf(light_attenuation) > 0.0f ){
        float3 Lc = light.color * light_attenuation;
        color += Kd * nDl * Lc;

        float3 H = normalize(L - ray.direction);
        float nDh = dot( ffnormal, H );
        if(nDh > 0)
          color += Ks * Lc * pow(nDh, phong_exp);
      }

    }
  }
  prd_radiance.result = color;
}

//
// (NEW)
// Attenuates shadow rays for shadowing transparent objects
//

rtDeclareVariable(float3, shadow_attenuation, , );

RT_PROGRAM void glass_any_hit_shadow()
{
  float3 world_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
  float nDi = fabs(dot(world_normal, ray.direction));

  prd_shadow.attenuation *= 1-fresnel_schlick(nDi, 5, 1-shadow_attenuation, make_float3(1));

  rtIgnoreIntersection();
}

//
// Dielectric surface shader
//
rtDeclareVariable(int,      max_depth, , );
rtDeclareVariable(float,    importance_cutoff, , );      
rtDeclareVariable(float3,       cutoff_color, , );
rtDeclareVariable(float,        fresnel_exponent, , );
rtDeclareVariable(float,        fresnel_minimum, , );
rtDeclareVariable(float,        fresnel_maximum, , );
rtDeclareVariable(float,        refraction_index, , );
rtDeclareVariable(int,          refraction_maxdepth, , );
rtDeclareVariable(int,          reflection_maxdepth, , );
rtDeclareVariable(float3,       refraction_color, , );
rtDeclareVariable(float3,       reflection_color, , );
rtDeclareVariable(float3,       extinction_constant, , );
RT_PROGRAM void glass_closest_hit_radiance()
{
  // intersection vectors
  const float3 h = ray.origin + t_hit * ray.direction;            // hitpoint
  const float3 n = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal)); // normal
  const float3 i = ray.direction;                                            // incident direction

  float reflection = 1.0f;
  float3 result = make_float3(0.0f);

  float3 beer_attenuation;
  if(dot(n, ray.direction) > 0){
    // Beer's law attenuation
    beer_attenuation = exp(extinction_constant * t_hit);
  } else {
    beer_attenuation = make_float3(1);
  }

  // refraction
  if (prd_radiance.depth < min(refraction_maxdepth, max_depth))
  {
    float3 t;                                                            // transmission direction
    if ( refract(t, i, n, refraction_index) )
    {

      // check for external or internal reflection
      float cos_theta = dot(i, n);
      if (cos_theta < 0.0f)
        cos_theta = -cos_theta;
      else
        cos_theta = dot(t, n);

      reflection = fresnel_schlick(cos_theta, fresnel_exponent, fresnel_minimum, fresnel_maximum);

      float importance = prd_radiance.importance * (1.0f-reflection) * optix::luminance( refraction_color * beer_attenuation );
      if ( importance > importance_cutoff ) {
        optix::Ray ray( h, t, radiance_ray_type, scene_epsilon );
        PerRayData_radiance refr_prd;
        refr_prd.depth = prd_radiance.depth+1;
        refr_prd.importance = importance;

        rtTrace( top_object, ray, refr_prd );
        result += (1.0f - reflection) * refraction_color * refr_prd.result;
      } else {
        result += (1.0f - reflection) * refraction_color * cutoff_color;
      }
    }
    // else TIR
  }

  // reflection
  if (prd_radiance.depth < min(reflection_maxdepth, max_depth))
  {
    float3 r = reflect(i, n);

    float importance = prd_radiance.importance * reflection * optix::luminance( reflection_color * beer_attenuation );
    if ( importance > importance_cutoff ) {
      optix::Ray ray( h, r, radiance_ray_type, scene_epsilon );
      PerRayData_radiance refl_prd;
      refl_prd.depth = prd_radiance.depth+1;
      refl_prd.importance = importance;

      rtTrace( top_object, ray, refl_prd );
      result += reflection * reflection_color * refl_prd.result;
    } else {
      result += reflection * reflection_color * cutoff_color;
    }
  }

  result = result * beer_attenuation;

  prd_radiance.result = result;
}

//
// Set pixel to solid color upon failure
//
RT_PROGRAM void exception()
{
  output_buffer[launch_index] = make_color( bad_color );
}
