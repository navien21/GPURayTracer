#include <ParseJSONController.hpp>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <math.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using boost::property_tree::ptree;

namespace grt {

	ParseJSONController::ParseJSONController(Model & _model, const std::string & _jsonFile)
	: Controller(_model)
	, jsonFileName(_jsonFile)
	{ }

	//
	// Function to load in a json file into a RayTraceImage object.  If the input
	// filename is empty it will try to load in a file named "test.json"
	//
	// Input: filename with json file
	// Output: RayTraceImage with the data from the json file loaded
	//
	// Note no exception handling within this function or return value.
	// So if the json file is not there,
	// or if there is a json syntax error an exception will be thrown.
	// If not caught program will fail.
	//
	void ParseJSONController::execute() {

		ptree pt; // property tree
		if (jsonFileName.empty())
		{
			std::cout << "[WARN] - filename is empty.  Using test.json by default." << std::endl;
			jsonFileName = "test.json";
		}

		std::cout << "[DEBUG] - Trying to read input file: " << jsonFileName << std::endl;
		read_json(jsonFileName,pt);
		std::cout << "[DEBUG] - Loaded file!" << std::endl;

		// Read the title
		model.imageTitle = pt.get("image.title","GPURayTracer");

		// default to 1 if not given
		model.totalBrightness = (float) pt.get("image.totalBrightness",1.);

		// Lights
		boost::optional<ptree&> pt_lights = pt.get_child_optional("image.lights");

		if (pt_lights) {
			std::cout << "Loading lights from json file" << std::endl;
			// Now try to read in the lights
			BOOST_FOREACH( ptree::value_type &v, pt_lights.get() )
			{
				Light lightToAppend;
				lightToAppend.position.x = v.second.get<float>("xPos");
				lightToAppend.position.y = v.second.get<float>("yPos");
				lightToAppend.position.z = v.second.get<float>("zPos");
				lightToAppend.color.r = v.second.get<float>("r");
				lightToAppend.color.g = v.second.get<float>("g");
				lightToAppend.color.b = v.second.get<float>("b");
				model.lights.push_back(lightToAppend);
			}
		}

		// Boxes
		boost::optional<ptree&> pt_boxes = pt.get_child_optional("image.boxes");

		if (pt_boxes) {
			std::cout << "Loading boxes from json file" << std::endl;
			// Now try to read in the boxes
			BOOST_FOREACH( ptree::value_type &v, pt_boxes.get() )
			{
				Box boxToAppend;
				boxToAppend.minPosition.x = v.second.get<float>("MinXPos");
				boxToAppend.minPosition.y = v.second.get<float>("MinYPos");
				boxToAppend.minPosition.z = v.second.get<float>("MinZPos");
				boxToAppend.maxPosition.x = v.second.get<float>("MaxXPos");
				boxToAppend.maxPosition.y = v.second.get<float>("MaxYPos");
				boxToAppend.maxPosition.z = v.second.get<float>("MaxZPos");
				boxToAppend.material = v.second.get<std::string>("material","phong");
				model.boxes.push_back(boxToAppend);
			}
		}

		// Spheres
		boost::optional<ptree&> pt_spheres = pt.get_child_optional("image.spheres");

		if (pt_spheres) {
			std::cout << "Loading spheres from json file" << std::endl;
			// Now try to read in the spheres
			BOOST_FOREACH( ptree::value_type &v, pt_spheres.get() )
			{
				Sphere sphereToAppend;
				sphereToAppend.position.x = v.second.get<float>("CenterXPos");
				sphereToAppend.position.y = v.second.get<float>("CenterYPos");
				sphereToAppend.position.z = v.second.get<float>("CenterZPos");
				sphereToAppend.radius = v.second.get<float>("radius");
				sphereToAppend.material = v.second.get<std::string>("material","phong");
				model.spheres.push_back(sphereToAppend);
			}
		}

		// Chulls
		boost::optional<ptree&> pt_chulls = pt.get_child_optional("image.chulls");

		if (pt_chulls) {
			std::cout << "Loading chulls from json file" << std::endl;
			// Now try to read in the chulls
			BOOST_FOREACH( ptree::value_type &v, pt_chulls.get() )
			{
				Chull chullToAppend;
				chullToAppend.position.x = v.second.get<float>("CenterXPos");
				chullToAppend.position.y = v.second.get<float>("CenterYPos");
				chullToAppend.position.z = v.second.get<float>("CenterZPos");
				chullToAppend.radius = v.second.get<float>("radius");
				chullToAppend.min = v.second.get<float>("min");
				chullToAppend.max = v.second.get<float>("max");
				chullToAppend.nsides = v.second.get<int>("nsides");
				chullToAppend.material = v.second.get<std::string>("material","phong");
				model.chulls.push_back(chullToAppend);
			}
		}

		// Check and set defaults here
		if (!pt_lights) {
			if (pt.get("image.useDefaultLights",true)) {
				std::cout << "[INFO] - Using Default Lights" << std::endl;
				// Save Default Lights
				Light l;
				{
					l.position.x = -25.0;
					l.position.y =  60.0;
					l.position.z = -16.0;
					l.color.r = 1;
					l.color.g = 1;
					l.color.b = 1;
				}
				model.lights.push_back(l);
				{
					l.position.x = -20.0;
					l.position.y =  60.0;
					l.position.z =  16.0;
					l.color.r = 1;
					l.color.g = 1;
					l.color.b = 1;
				}
				model.lights.push_back(l);
			} else {
				std::cout << "[WARN] - No Lights and default disabled!" << std::endl;
			}
		}

		if (!pt_boxes && pt.get("image.useDefaultBox",false)) {
			std::cout << "[INFO] - Using Default Box" << std::endl;
			// Save Default Box
			Box b;
			{
				b.minPosition.x = -2.0f;
				b.minPosition.y =  0.0f;
				b.minPosition.z = -2.0f;
				b.maxPosition.x =  2.0f;
				b.maxPosition.y =  7.0f;
				b.maxPosition.z =  2.0f;
			}
			model.boxes.push_back(b);
		}

		if (!pt_spheres && pt.get("image.useDefaultSphere",false)) {
			std::cout << "[INFO] - Using Default Sphere" << std::endl;
			Sphere s;
			{
				s.position.x =  2.0f;
				s.position.y =  1.5f;
				s.position.z = -2.5f;
				s.radius = 1.0f;
			}
			model.spheres.push_back(s);
		}

		if (model.boxes.empty() && model.spheres.empty()) {
			std::cout << "[WARN] - No Shapes and defaults disabled!" << std::endl;
		}

		std::cout << "[DEBUG] - Image data loaded from file "  << jsonFileName << ":" << std::endl << model << std::endl;

		std::cout << "[INFO] - ParseJSONController calling model.notifyObservers" << std::endl;
		model.notifyObservers();
	}

}; // grt namespace
