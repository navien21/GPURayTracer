#include <ParseJSONController.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

namespace grt {

	ParseJSONController::ParseJSONController(Model & _model, const std::string & _jsonFile)
	: Controller(_model)
	{
		boost::property_tree::read_json(_jsonFile,jsonPtree);
	}

	void ParseJSONController::execute() {
		if (parseModel()) model.notifyObservers();
	}

	bool ParseJSONController::parseModel() {

		if ( jsonPtree.get("image.useDefaultLights",true) ) return true;

		// default to 1
		model.lighting.totalBrightness = jsonPtree.get("image.totalBrightness",1.);

		// Now try to read in the lights
		BOOST_FOREACH( boost::property_tree::ptree::value_type &v, jsonPtree.get_child("image.lights") )
		{
			Light lightToAppend;
			lightToAppend.position.x = v.second.get<float>("xPos");
			lightToAppend.position.y = v.second.get<float>("yPos");
			lightToAppend.position.z = v.second.get<float>("zPos");
			lightToAppend.color.r = v.second.get<float>("r");
			lightToAppend.color.g = v.second.get<float>("g");
			lightToAppend.color.b = v.second.get<float>("b");
			model.lighting.lights.push_back(lightToAppend);
		}

		return true;
	}

};
