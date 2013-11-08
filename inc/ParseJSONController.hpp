#pragma once

#include <Controller.hpp>

#include <boost/property_tree/ptree.hpp>

namespace grt {

	class ParseJSONController
	: public Controller
	{
		protected:
			boost::property_tree::ptree jsonPtree;

			bool parseModel();

		public:
			ParseJSONController(Model & _model, const std::string & _jsonFile);

			void execute();


	};

}; // grt
