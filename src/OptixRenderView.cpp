

#include <iostream>
#include <OptiXRenderView.hpp>

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

#include <vector>

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
	// set up path to ptx file
	m_ptx_path = ptxpath( "gpuRayTracer", "tutorial3.cu" );

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
	std::string camera_name = "pinhole_camera";
	Program ray_gen_program = m_context->createProgramFromPTXFile( m_ptx_path, camera_name );
	m_context->setRayGenerationProgram( 0, ray_gen_program );

	// Exception / miss programs
	Program exception_program = m_context->createProgramFromPTXFile( m_ptx_path, "exception" );
	m_context->setExceptionProgram( 0, exception_program );
	m_context["bad_color"]->setFloat( 0.0f, 1.0f, 0.0f );

	std::string miss_name = "miss";
	m_context->setMissProgram( 0, m_context->createProgramFromPTXFile( m_ptx_path, miss_name ) );
	const float3 default_color = make_float3(1.0f, 1.0f, 1.0f);
	//m_context["envmap"]->setTextureSampler( loadTexture( m_context, texpath("CedarCity.hdr"), default_color) );
	m_context["bg_color"]->setFloat( make_float3( 0.34f, 0.55f, 0.85f ) );

	std::vector<BasicLight> lights;

	std::vector<grt::Light> & json_lights = model.lights;
	int n = json_lights.size(); //number of light sources... comes from JSON
	float brightness = model.totalBrightness;

	// temporary float arrays... these should be their own struct or class so they can be
	// looped through to generate light sources.. or just directly parsed into BasicLight form
	lights.resize(n);
	for(int i = 0; i < n; i++) {
		grt::Light currentLight = json_lights[i];
		float lightposition[3] = {currentLight.position.x, currentLight.position.y, currentLight.position.z};
		float lightcolor[3] = {currentLight.color.r, currentLight.color.g, currentLight.color.b};
		lights[i] = generateLight(lightposition, lightcolor);
	}

	normalizeLight(lights.data(), n, brightness); // corrects for brightness

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

	// Create boxes
	std::vector< Geometry > boxes;
	for (int iBox=0; iBox<(int)model.boxes.size(); ++iBox)
	{
		Geometry box = m_context->createGeometry();
		box->setPrimitiveCount( 1u );
		box->setBoundingBoxProgram( box_bounds );
		box->setIntersectionProgram( box_intersect );
		box["boxmin"]->setFloat( model.boxes[iBox].minPosition.x,
				model.boxes[iBox].minPosition.y,
				model.boxes[iBox].minPosition.z);
		box["boxmax"]->setFloat( model.boxes[iBox].maxPosition.x,
				model.boxes[iBox].maxPosition.y,
				model.boxes[iBox].maxPosition.z);
		boxes.push_back(box);
	}

	std::string sphere_ptx( ptxpath( "gpuRayTracer", "sphere.cu" ) );
	Program sphere_bounds = m_context->createProgramFromPTXFile( sphere_ptx, "bounds" );
	Program sphere_intersect = m_context->createProgramFromPTXFile( sphere_ptx, "robust_intersect" );

	// Create Spheres
	std::vector< Geometry > spheres;
	for (int iSphere=0; iSphere<(int)model.spheres.size(); ++iSphere)
	{
		Geometry sphere = m_context->createGeometry();
		sphere->setPrimitiveCount( 1u );
		sphere->setBoundingBoxProgram( sphere_bounds );
		sphere->setIntersectionProgram( sphere_intersect );
		sphere["sphere"]->setFloat( model.spheres[iSphere].position.x
				, model.spheres[iSphere].position.y
				, model.spheres[iSphere].position.z
				, model.spheres[iSphere].radius
				);
		spheres.push_back(sphere);
	}

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
	std::string box_chname = "closest_hit_radiance3";
	std::string floor_chname = "closest_hit_radiance3";
	Material floor_matl = m_context->createMaterial();
	Program floor_ch = m_context->createProgramFromPTXFile( m_ptx_path, floor_chname );
	floor_matl->setClosestHitProgram( 0, floor_ch );
	Program floor_ah = m_context->createProgramFromPTXFile( m_ptx_path, "any_hit_shadow" );
	floor_matl->setAnyHitProgram( 1, floor_ah );
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
	for (int iBox=0; iBox<(int)boxes.size();++iBox)
	{
		Material box_matl = m_context->createMaterial();
		Program box_ch = m_context->createProgramFromPTXFile( m_ptx_path, box_chname );
		box_matl->setClosestHitProgram( 0, box_ch );
		Program box_ah = m_context->createProgramFromPTXFile( m_ptx_path, "any_hit_shadow" );
		box_matl->setAnyHitProgram( 1, box_ah );
		box_matl["Ka"]->setFloat( 0.3f, 0.3f, 0.3f );
		box_matl["Kd"]->setFloat( 0.6f, 0.7f, 0.8f );
		box_matl["Ks"]->setFloat( 0.8f, 0.9f, 0.8f );
		box_matl["phong_exp"]->setFloat( 88 );
		box_matl["reflectivity_n"]->setFloat( 0.2f, 0.2f, 0.2f );

		gis.push_back( m_context->createGeometryInstance( boxes[iBox], &box_matl, &box_matl+1 ) );
	}
	for (int iSphere=0; iSphere<(int)spheres.size();++iSphere)
	{
		Material sphere_matl = m_context->createMaterial();
		Program sphere_ch = m_context->createProgramFromPTXFile( m_ptx_path, box_chname );
		sphere_matl->setClosestHitProgram( 0, sphere_ch );
		Program sphere_ah = m_context->createProgramFromPTXFile( m_ptx_path, "any_hit_shadow" );
		sphere_matl->setAnyHitProgram( 1, sphere_ah );
		sphere_matl["Ka"]->setFloat( 0.2f, 0.5f, 0.5f );
		sphere_matl["Kd"]->setFloat( 0.2f, 0.7f, 0.8f );
		sphere_matl["Ks"]->setFloat( 0.9f, 0.9f, 0.9f );
		sphere_matl["phong_exp"]->setFloat( 64 );
		sphere_matl["reflectivity_n"]->setFloat( 0.5f, 0.5f, 0.5f );

		gis.push_back( m_context->createGeometryInstance( spheres[iSphere], &sphere_matl, &sphere_matl+1 ) );
	}
	gis.push_back( m_context->createGeometryInstance( parallelogram, &floor_matl, &floor_matl+1 ) );

	// Place all in group
	GeometryGroup geometrygroup = m_context->createGeometryGroup();
	geometrygroup->setChildCount( static_cast<unsigned int>(gis.size()) );
	for (int i=0; i<(int)gis.size();++i)
		geometrygroup->setChild( i, gis[i] );

	geometrygroup->setAcceleration( m_context->createAcceleration("NoAccel","NoAccel") );

	m_context["top_object"]->set( geometrygroup );
	m_context["top_shadower"]->set( geometrygroup );
}


namespace grt {

	OptiXRenderView::OptiXRenderView(int argc, char ** argv) {
		std::cout<<"Notes for GLUT usage. Includes keystrokes and possible command line arguments"<<std::endl;
		GLUTDisplay::printUsage();
		GLUTDisplay::init( argc,  argv);
	}

	void OptiXRenderView::modelUpdated(const Model &model)
	{
		std::cout << "[INFO] - Model update received by optixRenderView object."<<std::endl;

		try {
			Tutorial scene(model);
			GLUTDisplay::run( model.imageTitle, &scene );
		} catch( Exception& e ){
			sutilReportError( e.getErrorString().c_str() );
			exit(1);
		}
	}

}; // grt namespace

