/*
 * Includes items necessary to construct an image or scene.
 * Note prints functions could be set up to work as << operators
 * but don't remember how to do that right now.
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
        public:
            ParseJSONController(Model & _model, const std::string & _jsonFile)
            : Controller(_model)
            {
                boost::property_tree::read_json(_jsonFile,jsonPtree);
				LoadRayTraceImageFromJsonFile(_jsonFile, model.image);
            }

            void execute() {
 
                // let everyone know
				std::cout<<"ParseJSONController calling model.notifyObservers"<<std::endl;
                model.notifyObservers();
            }
    };

};
  

#endif

