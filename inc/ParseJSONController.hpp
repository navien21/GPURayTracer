/*
 Includes the ParseJSONController class declaration.  It inherits from Controller
 To use:
 Construct with a Model object and JSON file name, then execute.
 */

#ifndef GPU_RAY_TRACER_PARSE_JSON_CONTROLLER_H
#define GPU_RAY_TRACER_PARSE_JSON_CONTROLLER_H

#include "RayTraceImageData.h"
#include "jsonParameterReader.h"
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <Model.hpp>
#include <Controller.hpp>

namespace grt {

    class ParseJSONController
    : public Controller
    {
        protected:
            boost::property_tree::ptree jsonPtree;
			std::string jsonFileName;
        public:
            ParseJSONController(Model & _model, const std::string &_jsonFile)
            : Controller(_model), jsonFileName(_jsonFile)
            {
                
            }

			void updateModelFromJSONFile()
			{
				// Try to load the JSON file.
				boost::property_tree::read_json(jsonFileName,jsonPtree);
				// This updates the image portion of the Model.
				LoadRayTraceImageFromJsonFile(jsonFileName, model.image);
			}
            void execute() {
				// Update the model with the JSON information
				std::cout<<"ParseJSONController refreshing Model Information from JSON file "<<jsonFileName<<std::endl;
				updateModelFromJSONFile();
 
                // let everyone know
				std::cout<<"ParseJSONController calling model.notifyObservers"<<std::endl;
                model.notifyObservers();
            }
    };

};
  

#endif

