/*
 * Includes items necessary to construct an image or scene.
 * Note prints functions could be set up to work as << operators
 * but don't remember how to do that right now.
 */

#ifndef GPU_RAY_TRACER_OPTIX_RENDER_VIEW_H
#define GPU_RAY_TRACER_OPTIX_RENDER_VIEW_H

#include "RayTraceImageData.h"
#include "jsonParameterReader.h"
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <Model.hpp>
#include <Controller.hpp>

namespace grt {

   

	class OptiXRenderView
		: public IModelObserver
	{
	private:
		int argc;
		char **argv;
	public:
		OptiXRenderView(int inArgc, char *inArgv[])
			: argv(inArgv),argc(inArgc)
		{ }
		void setUpAndDisplayImageWithOptiX(const Model &model);
		void modelUpdated(const Model &_model )
		{
			std::cout<<"Model update received by optixRenderView object."<<std::endl;
			setUpAndDisplayImageWithOptiX(_model);
		}
	};


};
  

#endif

