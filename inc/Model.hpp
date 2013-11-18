/*
 * Includes items necessary to construct an image or scene.
 * Note prints functions could be set up to work as << operators
 * but don't remember how to do that right now.
 */

#ifndef MODEL_H
#define MODEL_H

#include "RayTraceImageData.h"
#include "jsonParameterReader.h"
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


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
  

#endif

