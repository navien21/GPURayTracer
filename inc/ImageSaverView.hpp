/* ImageSaverView class declaration.  Inherits from IModelObserver
*  Saves an image to a file.  Not implemented yet */

#pragma once

#include <Model.hpp>

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
                std::cout << "ImageSaverView Model Update received! [" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
            }
    };

};


