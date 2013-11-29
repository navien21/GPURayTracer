/*
   Includes the ParseJSONController class declaration.  It inherits from Controller
   To use:
   Construct with a Model object and JSON file name, then execute.
   */

#pragma once

#include <Model.hpp>
#include <Controller.hpp>

namespace grt {

	class ParseJSONController
		: public Controller
	{
		protected:
			std::string jsonFileName;
		public:
			ParseJSONController(Model & _model, const std::string &_jsonFile);
			void execute();
	};

};

