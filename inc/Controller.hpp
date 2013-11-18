/*
 * Includes items necessary to construct an image or scene.
 * Note prints functions could be set up to work as << operators
 * but don't remember how to do that right now.
 */

#ifndef GPU_RAY_TRACER_CONTROLLER_H
#define GPU_RAY_TRACER_CONTROLLER_H

#include "RayTraceImageData.h"
#include "jsonParameterReader.h"
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
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


};
  

#endif

