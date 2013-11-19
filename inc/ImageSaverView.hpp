#pragma once

#include <Model.hpp>
#include <iostream>

namespace grt {

	class ImageSaverView
		: public IModelObserver
	{
		private:
			std::string filename;
		public:
			ImageSaverView(const std::string & _filename)
				: filename(_filename)
			{}

			void modelUpdated(const Model & _model) {
				// TODO : save to file
				std::cout << "Model Update received! [" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
			}
	};

}; // grt
