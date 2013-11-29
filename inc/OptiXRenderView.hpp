/* Includes the OptiXRenderView object */

#pragma once

#include <Model.hpp>

namespace grt {

	class OptiXRenderView
		: public IModelObserver
	{
		public:
			OptiXRenderView(int inArgc, char ** inArgv);
			void modelUpdated(const Model & _model);
	};

};


