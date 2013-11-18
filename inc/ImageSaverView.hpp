

#ifndef GPU_RAY_TRACER_IMAGE_SAVER_VIEW_H
#define GPU_RAY_TRACER_IMAGE_SAVER_VIEW_H

#include "RayTraceImageData.h"
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <Model.hpp>
#include <Controller.hpp>

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

};

#endif

