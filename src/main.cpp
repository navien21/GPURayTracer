
#include <iostream>
#include <set>

//#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//#include <Scene.hpp>
//#include <Image.hpp>
#include <GPUManager.hpp>
#include <OptiXRenderView.hpp>
#include <ImageSaverView.hpp>
#include <ParseJSONController.hpp>



int main(int argc, char ** argv)
{
    std::string json_file;
    std::string img_file;

    // Setup program options
    /*boost::program_options::options_description desc("Allowed options");
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
	*/

	if (argc<2)
	{
		std::cout<<"Usage: "<<argv[0]<<" json_file img_file"<<std::endl;
		std::cout<<"Where json_file is the path to the input json parameter file that describing the scene (required)"<<std::endl;
		std::cout<<"And img_file is the path to save the rendered png file (optional)"<<std::endl;
	}
	else 
	{
		json_file = argv[1];
	}
	if (argc>2)
	{
		img_file = argv[2];
	}
	std::cout<<"Got command line arguments json_file = "<<json_file<<", img_file = "<<img_file<<std::endl;

    // Create a model
    grt::Model model;

	// Setup optix renderer
	grt::OptiXRenderView optiXRenderView(argc, argv);
	model.registerObserver(&optiXRenderView);

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
