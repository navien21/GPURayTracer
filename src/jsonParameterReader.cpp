
/*

   This file contains the LoadRayTraceImageFromJsonFile
   definition that loads in data from a json file
   to a RayTraceImageData object.

   Example json file contents with 2 lights and the default box:
   {
   "image":
   {
   "useDefaultLights": false,
   "totalBrightness": 1.0,
   "lights":
   [

   { "xPos": -25.0, "yPos": 60.0, "zPos": -16.0, 
   "r": 1.0, "g": 1.0, "b": 1.0},

   { "xPos":-20.0, "yPos": 60.0, "zPos": 16.0,
   "r": 1.0, "g": 1.0, "b": 1.0}
   ]
   }
   }

*/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <math.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include "RayTraceImageData.h"

using boost::property_tree::ptree;

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
void LoadRayTraceImageFromJsonFile(std::string fileName, RayTraceImageData &image)
{
	std::cout<<"inside LoadRayTraceImageFromJsonFile!"<<std::endl;

	ptree pt; // property tree
	if (fileName.empty())
	{
		std::cout<<"filename is empty.  Using test.json by default."<<std::endl;
		fileName = "test.json";
	}
	std::cout<<"Trying to load in file "<<fileName<<" to boost property tree structure."<<std::endl;
	read_json(fileName,pt);
	std::cout<<"Loaded file!"<<std::endl;

	// Read the title
	image.imageTitle = pt.get("image.title","GPURayTracer");

	// Should default lighting be used?
	image.useDefaultLighting = pt.get("image.useDefaultLights",true);


	// Go ahead and try to read in the rest of the lighting information
	if ( !image.useDefaultLighting )
	{
		std::cout<<"image.useDefaultLighting false.  "
			<<"Will try to load light information from json file"<<std::endl;

		// default to 1 
		image.lighting.totalBrightness = (float) pt.get("image.totalBrightness",1.);

		// Now try to read in the lights
		BOOST_FOREACH( ptree::value_type &v, pt.get_child("image.lights") )
		{
			Light lightToAppend;
			lightToAppend.position.x = v.second.get<float>("xPos");
			lightToAppend.position.y = v.second.get<float>("yPos");
			lightToAppend.position.z = v.second.get<float>("zPos");
			lightToAppend.color.r = v.second.get<float>("r");
			lightToAppend.color.g = v.second.get<float>("g");
			lightToAppend.color.b = v.second.get<float>("b");
			image.lighting.lights.push_back(lightToAppend);
		}
	}

	// Should the default box be used
	image.useDefaultBox = pt.get("image.useDefaultBox",true);

	if ( !image.useDefaultBox )
	{
		std::cout<<"image.useDefaultBox is false.  "
			<<"Will try to load box information from json file"<<std::endl;

		// Now try to read in the boxes
		BOOST_FOREACH( ptree::value_type &v, pt.get_child("image.boxes") )
		{
			Box boxToAppend;
			boxToAppend.minPosition.x = v.second.get<float>("MinXPos");
			boxToAppend.minPosition.y = v.second.get<float>("MinYPos");
			boxToAppend.minPosition.z = v.second.get<float>("MinZPos");
			boxToAppend.maxPosition.x = v.second.get<float>("MaxXPos");
			boxToAppend.maxPosition.y = v.second.get<float>("MaxYPos");
			boxToAppend.maxPosition.z = v.second.get<float>("MaxZPos");
			image.boxes.push_back(boxToAppend);
		}

	}

	boost::optional<ptree&> pt_spheres = pt.get_child_optional("image.spheres");

	if (pt_spheres) {
		std::cout << "Loading spheres from json file" << std::endl;
		// Now try to read in the spheres
		BOOST_FOREACH( ptree::value_type &v, pt.get_child("image.spheres") )
		{
			Sphere sphereToAppend;
			sphereToAppend.position.x = v.second.get<float>("CenterXPos");
			sphereToAppend.position.y = v.second.get<float>("CenterYPos");
			sphereToAppend.position.z = v.second.get<float>("CenterZPos");
			sphereToAppend.radius = v.second.get<float>("radius");
			image.spheres.push_back(sphereToAppend);
		}
	}

	// TODO : Check and set defaults here

	std::cout<<"Image data loaded from file "<<fileName<<std::endl;
	image.print();
}
