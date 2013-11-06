/*
 * Includes items necessary to construct an image or scene.
 * Note prints functions could be set up to work as << operators
 * but don't remember how to do that right now.
 */

#ifndef GPU_RAY_TRACER_H
#define GPU_RAY_TRACER_H

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

	class OptixRenderView
		: public IModelObserver
	{
	private:
		int argc;
		char **argv;
	public:
		OptixRenderView(int inArgc, char *inArgv[])
			: argv(inArgv),argc(inArgc)
		{ }
		void setUpAndDisplayImageWithOptix(const Model &model);
		void modelUpdated(const Model &_model )
		{
			std::cout<<"Model update received by optixRenderView object."<<std::endl;
			setUpAndDisplayImageWithOptix(_model);
		}
	};


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

