
/*
 * Example json file contents with 2 lights:
 * 
{
  "image":
  {
     "useDefaultLights": false,
     "totalLightAmount": { "r": 20, "g": 30, "b": 50 },
     "lights":
     [

       { "xPos": 5, "yPos": 10, "zPos": 15, 
         "r": 255, "g": 255, "b": 255 },

       { "xPos":25, "yPos": 30, "zPos": 35,
         "r": 10, "g": 10, "b": 10 }
     ]
  }
}


 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <math.h>

/*
#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
*/

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
 
  image.useDefaultLighting = pt.get("image.useDefaultLights",true);
  
  // Go ahead and try to read in the rest of the lighting information
  if ( !image.useDefaultLighting )
  {
	  image.useDefaultLighting = false;
	  std::cout<<"image.useDefaultLighting false.  "
		  <<"Will try to load light information from json file"<<std::endl;

	  // defaults set to 100 for now.  This is arbitrary
	  image.lighting.totalLightAmount.r = pt.get("image.totalLightAmount.r",100.);
	  image.lighting.totalLightAmount.g = pt.get("image.totalLightAmount.g", 100.);
	  image.lighting.totalLightAmount.b = pt.get("image.totalLightAmount.b", 100.);

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

  std::cout<<"Image data loaded from file "<<fileName<<std::endl;
  image.print();


}
