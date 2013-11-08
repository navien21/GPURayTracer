#pragma once

#include <Model.hpp>

namespace grt {

	class Controller {
		protected:
			Model & model;
		public:
			Controller(Model & _model)
				: model(_model)
			{}

			void execute() {}

			// NOT IMPLEMENTED YET
			void revert() {}
	};

}; // grt
