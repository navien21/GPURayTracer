/*
 Includes the Model and IModelObserver class declarations.

 The Model has the items necessary to construct a modified OptiX image.

 In normal use, one or more IModelObserver objects are registered
 to the model.  Specific observers or viewers may inherit from 
 IModelObservers.

 A controller updates the model and calls the model notifyObservers
 function which calls the modelUpdated function of each of the
 observers registered to the model.

*/

#pragma once

#include "RayTraceImageData.h"
#include <set>


namespace grt {

    class Model;

    class IModelObserver {
        public:
            virtual void modelUpdated(const Model &) = 0;
    };

    class Model {
        std::set<IModelObserver*> observers;

        public:
			RayTraceImageData image;

            void registerObserver(IModelObserver * _obs) {
                observers.insert(_obs);
            }

            void unregisterObserver(IModelObserver * _obs) {
                observers.erase(_obs);
            }

            void notifyObservers() {
				std::cout<<"Inside a Model class notifyObservers function"<<std::endl;
                for (std::set<IModelObserver*>::iterator ob_it = observers.begin(); ob_it != observers.end(); ++ob_it) {
					std::cout<<"Notifing an observer!"<<std::endl;
                    (*ob_it)->modelUpdated(*this);
                }
            }
    };

}; // grt
  
