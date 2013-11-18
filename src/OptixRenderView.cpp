

#include <iostream>
#include <OptiXRenderView.hpp>

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_namespace.h>
#include <iostream>
#include <GLUTDisplay.h>
#include <ImageLoader.h>
//#include "commonStructs.h"
#include "GPUManager.hpp"
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <math.h>

//#include "Tutorial.h"
#include <vector>

#include "RayTraceImageData.h"
#include "jsonParameterReader.h"

#include <TutorialClass.hpp>

using namespace optix;


static float rand_range(float min, float max)
{
  return min + (max - min) * (float) rand() / (float) RAND_MAX;
}


// added by Dylan (begin)
BasicLight generateLight(float pos[3],float color[3]){
    BasicLight light;
    light.casts_shadow=1;
    light.padding=0;
    light.color = make_float3(color[0],color[1],color[2]);
    light.pos = make_float3(pos[0],pos[1],pos[2]);
    return light;
}

void normalizeLight(BasicLight lights[], int n, float brightness=1){
    float norm;
    float btotal=0;
    for (int i=0; i!=n; i++){
        btotal += lights[i].color.x+ lights[i].color.y + lights[i].color.z;
    }
    norm = (3*brightness)/btotal;
    for(int i=0;i!=n;i++){
        lights[i].color.x = lights[i].color.x*norm;
        lights[i].color.y = lights[i].color.y*norm;
        lights[i].color.z = lights[i].color.z*norm;
    }
}
// added by Dylan (end)

void Tutorial::initScene( InitialCameraData& camera_data )
{
  // set up path to ptx file associated with tutorial number
  std::stringstream ss;
  ss << "tutorial" << m_tutnum << ".cu";
  m_ptx_path = ptxpath( "gpuRayTracer", ss.str() );

  // context 
  m_context->setRayTypeCount( 2 );
  m_context->setEntryPointCount( 1 );
  m_context->setStackSize( 4640 );

  m_context["max_depth"]->setInt(100);
  m_context["radiance_ray_type"]->setUint(0);
  m_context["shadow_ray_type"]->setUint(1);
  m_context["frame_number"]->setUint( 0u );
  m_context["scene_epsilon"]->setFloat( 1.e-3f );
  m_context["importance_cutoff"]->setFloat( 0.01f );
  m_context["ambient_light_color"]->setFloat( 0.31f, 0.33f, 0.28f );

  m_context["output_buffer"]->set( createOutputBuffer(RT_FORMAT_UNSIGNED_BYTE4, m_width, m_height) );

  // Ray gen program
  std::string camera_name;
  if(m_tutnum >= 11){
    camera_name = "env_camera";
  } else {
    camera_name = "pinhole_camera";
  }
  Program ray_gen_program = m_context->createProgramFromPTXFile( m_ptx_path, camera_name );
  m_context->setRayGenerationProgram( 0, ray_gen_program );

  // Exception / miss programs
  Program exception_program = m_context->createProgramFromPTXFile( m_ptx_path, "exception" );
  m_context->setExceptionProgram( 0, exception_program );
  m_context["bad_color"]->setFloat( 0.0f, 1.0f, 0.0f );

  std::string miss_name;
  if(m_tutnum >= 5)
    miss_name = "envmap_miss";
  else
    miss_name = "miss";
  m_context->setMissProgram( 0, m_context->createProgramFromPTXFile( m_ptx_path, miss_name ) );
  const float3 default_color = make_float3(1.0f, 1.0f, 1.0f);
  //m_context["envmap"]->setTextureSampler( loadTexture( m_context, texpath("CedarCity.hdr"), default_color) );
  m_context["bg_color"]->setFloat( make_float3( 0.34f, 0.55f, 0.85f ) );
  
  // Lights
  // Lights
  //BasicLight lights[1] = { 
  //  { make_float3( -25.0f, 60.0f, -16.0f ), make_float3( 1.0f, 1.0f, 1.0f ), 1 }
  //};


  std::vector<BasicLight> lights;
  float brightness = 1; // brightness... default is 1, but should be modifiable in JSON
  int n;

  if(!imageData.useDefaultLighting) {
	  std::vector<Light> json_lights = imageData.lighting.lights;
	  n=json_lights.size(); //number of light sources... comes from JSON
      brightness = imageData.lighting.totalBrightness; 

  // temporary float arrays... these should be their own struct or class so they can be
  // looped through to generate light sources.. or just directly parsed into BasicLight form
	  lights.resize(n);
  for(int i = 0; i < n; i++) {
	  Light currentLight = json_lights[i];
	  float lightposition[3] = {currentLight.position.x, currentLight.position.y, currentLight.position.z};
	  float lightcolor[3] = {currentLight.color.r, currentLight.color.g, currentLight.color.b};
	  lights[i] = generateLight(lightposition, lightcolor);

  }

  } else {

	  n=2; //number of light sources... comes from JSON
	  lights.resize(n);
  

  // temporary float arrays... these should be their own struct or class so they can be
  // looped through to generate light sources.. or just directly parsed into BasicLight form
  float light1pos[3] = {-25.0,60.0,-16.0};
  float light1color[3] = {1,1,1};

  float light2pos[3] = {-20.0,60.0, 16.0};
  float light2color[3] = {1,1,1};


  //BasicLight lights[2];
  
  
  //lights = new BasicLight[n];
  lights[0] = generateLight(light1pos,light1color);
  lights[1] = generateLight(light2pos,light2color);
  }

  normalizeLight(lights.data(), n, brightness); // corrects for brightness
  
  //lights[0].pos = make_float3( -25.0f, 60.0f, -16.0f );
  //lights[0].color = make_float3( 0.4f, 0.7f, 0.4f );

  
  
  Buffer light_buffer = m_context->createBuffer(RT_BUFFER_INPUT);
  light_buffer->setFormat(RT_FORMAT_USER);
  light_buffer->setElementSize(sizeof(BasicLight));
  light_buffer->setSize(lights.size());
  memcpy(light_buffer->map(), lights.data(), sizeof(lights[0])*lights.size());
  light_buffer->unmap(); 
    
  m_context["lights"]->set(light_buffer);
  
  // Set up camera
  camera_data = InitialCameraData( make_float3( 7.0f, 9.2f, -6.0f ), // eye
                                   make_float3( 0.0f, 4.0f,  0.0f ), // lookat
                                   make_float3( 0.0f, 1.0f,  0.0f ), // up
                                   60.0f );                          // vfov

  m_context["eye"]->setFloat( make_float3( 0.0f, 0.0f, 0.0f ) );
  m_context["U"]->setFloat( make_float3( 0.0f, 0.0f, 0.0f ) );
  m_context["V"]->setFloat( make_float3( 0.0f, 0.0f, 0.0f ) );
  m_context["W"]->setFloat( make_float3( 0.0f, 0.0f, 0.0f ) );

  // 3D solid noise buffer, 1 float channel, all entries in the range [0.0, 1.0].
  srand(0); // Make sure the pseudo random numbers are the same every run.

  int tex_width  = 64;
  int tex_height = 64;
  int tex_depth  = 64;
  Buffer noiseBuffer = m_context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT, tex_width, tex_height, tex_depth);
  float *tex_data = (float *) noiseBuffer->map();
  
  // Random noise in range [0, 1]
  for (int i = tex_width * tex_height * tex_depth;  i > 0; i--) {
    // One channel 3D noise in [0.0, 1.0] range.
    *tex_data++ = rand_range(0.0f, 1.0f);
  }
  noiseBuffer->unmap(); 


  // Noise texture sampler
  TextureSampler noiseSampler = m_context->createTextureSampler();

  noiseSampler->setWrapMode(0, RT_WRAP_REPEAT);
  noiseSampler->setWrapMode(1, RT_WRAP_REPEAT);
  noiseSampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
  noiseSampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
  noiseSampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
  noiseSampler->setMaxAnisotropy(1.0f);
  noiseSampler->setMipLevelCount(1);
  noiseSampler->setArraySize(1);
  noiseSampler->setBuffer(0, 0, noiseBuffer);

  m_context["noise_texture"]->setTextureSampler(noiseSampler);

  // Populate scene hierarchy
  createGeometry();

  // Prepare to run
  m_context->validate();
  m_context->compile();
}


Buffer Tutorial::getOutputBuffer()
{
  return m_context["output_buffer"]->getBuffer();
}


void Tutorial::trace( const RayGenCameraData& camera_data )
{
  m_context["eye"]->setFloat( camera_data.eye );
  m_context["U"]->setFloat( camera_data.U );
  m_context["V"]->setFloat( camera_data.V );
  m_context["W"]->setFloat( camera_data.W );

  Buffer buffer = m_context["output_buffer"]->getBuffer();
  RTsize buffer_width, buffer_height;
  buffer->getSize( buffer_width, buffer_height );

  m_context->launch( 0, static_cast<unsigned int>(buffer_width),
                      static_cast<unsigned int>(buffer_height) );
}


void Tutorial::doResize( unsigned int width, unsigned int height )
{
  // output buffer handled in SampleScene::resize
}

std::string Tutorial::texpath( const std::string& base )
{
  return texture_path + "/" + base;
}

float4 make_plane( float3 n, float3 p )
{
  n = normalize(n);
  float d = -dot(n, p);
  return make_float4( n, d );
}

void Tutorial::createGeometry()
{
  std::string box_ptx( ptxpath( "gpuRayTracer", "box.cu" ) ); 
  Program box_bounds = m_context->createProgramFromPTXFile( box_ptx, "box_bounds" );
  Program box_intersect = m_context->createProgramFromPTXFile( box_ptx, "box_intersect" );

  // Create box
  Geometry box = m_context->createGeometry();
  box->setPrimitiveCount( 1u );
  box->setBoundingBoxProgram( box_bounds );
  box->setIntersectionProgram( box_intersect );
  box["boxmin"]->setFloat( -2.0f, 0.0f, -2.0f );
  box["boxmax"]->setFloat(  2.0f, 7.0f,  2.0f );

  // Floor geometry
  std::string pgram_ptx( ptxpath( "gpuRayTracer", "parallelogram.cu" ) );
  Geometry parallelogram = m_context->createGeometry();
  parallelogram->setPrimitiveCount( 1u );
  parallelogram->setBoundingBoxProgram( m_context->createProgramFromPTXFile( pgram_ptx, "bounds" ) );
  parallelogram->setIntersectionProgram( m_context->createProgramFromPTXFile( pgram_ptx, "intersect" ) );
  float3 anchor = make_float3( -64.0f, 0.01f, -64.0f );
  float3 v1 = make_float3( 128.0f, 0.0f, 0.0f );
  float3 v2 = make_float3( 0.0f, 0.0f, 128.0f );
  float3 normal = cross( v2, v1 );
  normal = normalize( normal );
  float d = dot( normal, anchor );
  v1 *= 1.0f/dot( v1, v1 );
  v2 *= 1.0f/dot( v2, v2 );
  float4 plane = make_float4( normal, d );
  parallelogram["plane"]->setFloat( plane );
  parallelogram["v1"]->setFloat( v1 );
  parallelogram["v2"]->setFloat( v2 );
  parallelogram["anchor"]->setFloat( anchor );

  // Materials
  std::string box_chname;
  //if(m_tutnum >= 8){
  //  box_chname = "box_closest_hit_radiance";
  //} else if(m_tutnum >= 3){
    box_chname = "closest_hit_radiance3";
  //} else if(m_tutnum >= 2){
  //  box_chname = "closest_hit_radiance2";
  //} else if(m_tutnum >= 1){
  //  box_chname = "closest_hit_radiance1";
  //} else {
  //  box_chname = "closest_hit_radiance0";
  //}
  
  Material box_matl = m_context->createMaterial();
  Program box_ch = m_context->createProgramFromPTXFile( m_ptx_path, box_chname );
  box_matl->setClosestHitProgram( 0, box_ch );
  if(m_tutnum >= 3) {
    Program box_ah = m_context->createProgramFromPTXFile( m_ptx_path, "any_hit_shadow" );
    box_matl->setAnyHitProgram( 1, box_ah );
  }
  box_matl["Ka"]->setFloat( 0.3f, 0.3f, 0.3f );
  box_matl["Kd"]->setFloat( 0.6f, 0.7f, 0.8f );
  box_matl["Ks"]->setFloat( 0.8f, 0.9f, 0.8f );
  box_matl["phong_exp"]->setFloat( 88 );
  box_matl["reflectivity_n"]->setFloat( 0.2f, 0.2f, 0.2f );

  std::string floor_chname;
  //if(m_tutnum >= 7){
  //  floor_chname = "floor_closest_hit_radiance";
  //} else if(m_tutnum >= 6){
  //  floor_chname = "floor_closest_hit_radiance5";
  //} else if(m_tutnum >= 4){
  //  floor_chname = "floor_closest_hit_radiance4";
  //} else if(m_tutnum >= 3){
    floor_chname = "closest_hit_radiance3";
  //} else if(m_tutnum >= 2){
  //  floor_chname = "closest_hit_radiance2";
  //} else if(m_tutnum >= 1){
  //  floor_chname = "closest_hit_radiance1";
  //} else {
  //  floor_chname = "closest_hit_radiance0";
  //}
  //  
  Material floor_matl = m_context->createMaterial();
  Program floor_ch = m_context->createProgramFromPTXFile( m_ptx_path, floor_chname );
  floor_matl->setClosestHitProgram( 0, floor_ch );
  if(m_tutnum >= 3) {
    Program floor_ah = m_context->createProgramFromPTXFile( m_ptx_path, "any_hit_shadow" );
    floor_matl->setAnyHitProgram( 1, floor_ah );
  }
  floor_matl["Ka"]->setFloat( 0.3f, 0.3f, 0.1f );
  floor_matl["Kd"]->setFloat( 194/255.f*.6f, 186/255.f*.6f, 151/255.f*.6f );
  floor_matl["Ks"]->setFloat( 0.4f, 0.4f, 0.4f );
  floor_matl["reflectivity"]->setFloat( 0.1f, 0.1f, 0.1f );
  floor_matl["reflectivity_n"]->setFloat( 0.05f, 0.05f, 0.05f );
  floor_matl["phong_exp"]->setFloat( 88 );
  //floor_matl["tile_v0"]->setFloat( 0.25f, 0, .15f );
  //floor_matl["tile_v1"]->setFloat( -.15f, 0, 0.25f );
  //floor_matl["crack_color"]->setFloat( 0.1f, 0.1f, 0.1f );
  //floor_matl["crack_width"]->setFloat( 0.02f );

  // Create GIs for each piece of geometry
  std::vector<GeometryInstance> gis;
  gis.push_back( m_context->createGeometryInstance( box, &box_matl, &box_matl+1 ) );
  gis.push_back( m_context->createGeometryInstance( parallelogram, &floor_matl, &floor_matl+1 ) );

  // Place all in group
  GeometryGroup geometrygroup = m_context->createGeometryGroup();
  geometrygroup->setChildCount( static_cast<unsigned int>(gis.size()) );
  geometrygroup->setChild( 0, gis[0] );
  geometrygroup->setChild( 1, gis[1] );

  geometrygroup->setAcceleration( m_context->createAcceleration("NoAccel","NoAccel") );

  m_context["top_object"]->set( geometrygroup );
  m_context["top_shadower"]->set( geometrygroup );
}




namespace grt {
void OptiXRenderView::setUpAndDisplayImageWithOptiX(const Model &model)
{
	GLUTDisplay::init( argc,  argv);

  unsigned int width = 1080u, height = 720u;

  std::string texture_path;
  int tutnum = 3;

  std::stringstream title;
  title << "Tutorial " << tutnum;
  try {
    Tutorial scene(tutnum, texture_path, model.image);
    scene.setDimensions( width, height );
    GLUTDisplay::run( title.str(), &scene );
  } catch( Exception& e ){
    sutilReportError( e.getErrorString().c_str() );
    exit(1);
  }
  return;
}
	

}; // grt namespace



/*
 * Example json file contents with 2 lights:
 * 
{
  "image":
  {
     "useDefaultLights": false,
     "totalBrightness": 1,
     "lights":
     [

       { "xPos": 5, "yPos": 10, "zPos": 15, 
         "r": 255, "g": 255, "b": 255 },

       { "xPos":25, "yPos": 30, "zPos": 35,
         "r": 10, "g": 10, "b": 10 }
     ]
  }
}


 */
