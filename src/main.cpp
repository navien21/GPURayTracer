
#include <iostream>
#include <set>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <Scene.hpp>
#include <Image.hpp>
#include <GPUManager.hpp>

namespace grt {

    class Model;

    class IModelObserver {
        public:
            virtual void modelUpdated(const Model &) = 0;
    };

    class Model {
        std::set<IModelObserver*> observers;

        public:
            void registerObserver(IModelObserver * _obs) {
                observers.insert(_obs);
            }

            void unregisterObserver(IModelObserver * _obs) {
                observers.erase(_obs);
            }

            void notifyObservers() {
                for (std::set<IModelObserver*>::iterator ob_it = observers.begin(); ob_it != observers.end(); ++ob_it) {
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
            }

            void execute() {
                // TODO : Convert JSON PTree to Model

                // Finished changing model, let everyone know
                model.notifyObservers();
            }
    };

};

int main(int argc, char ** argv)
{
    std::string json_file;
    std::string img_file;

    // Setup program options
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h","display this text")
        ("jsonfile,j",boost::program_options::value<std::string>(&json_file)->required(),"path to the json file describing the scene to render")
        ("imgfile,i",boost::program_options::value<std::string>(&img_file),"path to save the rendered png file")
    ;

    // Parse program options
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc,argv,desc), vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    try {
        boost::program_options::notify(vm);
    } catch (std::exception & e) {
        std::cerr << "Program Options Parsing Error: " << e.what() << std::endl;
        return -1;
    }

    // Initialize GPU
    grt::GPUManager gman;

    // Create a model
    grt::Model model;

    // Setup Image Saver
    std::auto_ptr<grt::ImageSaverView> imagefileview( (img_file.empty()) ? 0 : new grt::ImageSaverView(img_file) );

    // Register views with Model
    //model.registerObserver(&gman);
    if (imagefileview.get()) model.registerObserver(imagefileview.get());

    try {
        grt::ParseJSONController jsonParser(model,json_file);
        jsonParser.execute(); // notifies model observers
    } catch (std::exception & e) {
        std::cerr << "Unable to ParseJSON: " << e.what() << std::endl;
        return -2;
    }

    // Cleanup

    // Register views with Model
    //model.unregisterObserver(&gman);
    if (imagefileview.get()) model.unregisterObserver(imagefileview.get());

    return 0;
}
