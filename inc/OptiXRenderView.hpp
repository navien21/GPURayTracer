#pragma once

#include <Model.hpp>

namespace grt {

	class OptixRenderView
	: public IModelObserver
	{
		public:
			OptixRenderView(int width, int height);

			void setUpAndDisplayImageWithOptix(const Model &model);

			void modelUpdated(const Model &);
	};

}; // grt
