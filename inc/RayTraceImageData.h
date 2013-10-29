/*
 * Includes items necessary to construct an image or scene.
 * Note prints functions could be set up to work as << operators
 * but don't remember how to do that right now.
 */

#ifndef RAY_TRACE_IMAGE_H
#define RAY_TRACE_IMAGE_H

class Position
{ 
public:
	float x;
	float y;
	float z;
	void print()
	{
		std::cout<<"position: x= "<<x<<", y= "<<y<<", z= "<<z<<std::endl;
	}
};

class Color
{
public:
	float r;
	float g;
	float b;
	void print()
	{
		std::cout<<"color: r= "<<r<<", g= "<<g<<", b= "<<b<<std::endl;
	}
};

class Light
{
public:
  Position position; // x, y, z position
  Color color; // R G B color

  void print()
  {
	  position.print();
	  color.print();
  }
};  

class Lighting
{
public:
    Color totalLightAmount;  
    std::vector< Light > lights;
	void print()
	{
		std::cout<<"totalLightAmount: ";
		totalLightAmount.print();
		int nLights = lights.size();
		for (int i=0; i<nLights; ++i)
		{
			std::cout<<"light number: "<<i<<std::endl;
			lights[i].print();
		}
	}
};


class Camera
{
public:
  Position position;
  float fieldOfView;
};

// May become a shape
class Box
{
public:
  Position minPosition;
  Position maxPosition;
  // Material would go here? Or maybe several colors?
};

// May become a shape
class Parallelogram
{
public:
  // how specify? vec1, vec2, anchor, Material?
};


class RayTraceImageData
{
public:
  bool useDefaultLighting;
  bool useDefaultCamera;
  bool useDefaultParallelogramFloor;
  bool useDefaultBox;

  // Lighting information
  Lighting lighting;

  /* Rest is not yet ready
  // Camera information
  Camera camera;
  
  // Box shapes
  std::vector< Box > box;
  
  // Parallelogram shapes (includes floor)
  std::vector< Parallelogram > parallelogram;
  */

  // Constructor.  Set everything to use defaults vectors are all size 0 by default
  // by default.
  RayTraceImageData():
  useDefaultLighting(true),
  useDefaultCamera(true),
  useDefaultParallelogramFloor(true),
  useDefaultBox(true)
    {
 
    }

  void print()
  {
	  std::cout<<"useDefaultLighting = "<<useDefaultLighting<<std::endl;
	  std::cout<<"useDefaultCamera = "<<useDefaultCamera<<std::endl;
	  std::cout<<"useDefaultParallelogramFloor = "<<useDefaultParallelogramFloor<<std::endl;
	  std::cout<<"useDefaultBox = "<<useDefaultBox<<std::endl;
	  if (!useDefaultLighting)
		lighting.print();
  }

  // Since no pointers the default assignment operator, copy constructor
  // and destructors should work OK.  Not specified.
};

  
  
  

#endif

