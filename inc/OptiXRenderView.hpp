/* Includes the OptiXRenderView object */

#pragma once

#include <Model.hpp>

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
  

