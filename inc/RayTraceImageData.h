/*
 * Includes items necessary to construct an image or scene.
 */


#pragma once


#include <vector>

class Position
{ 
public:
	float x;
	float y;
	float z;
	void print()
	{
		std::cout<<*this<<std::endl;

	}
	friend std::ostream & operator<<(std::ostream & _out, const Position & pos) 
	{
		_out << "position: x= "  << pos.x << ", y= " << pos.y << ", z= " << pos.z;
		return _out;
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
		std::cout<<*this<<std::endl;
	}
	friend std::ostream & operator<<(std::ostream & _out, const Color & c) 
	{
		_out << "color: r=" << c.r << ", g=" << c.g << ", b= " << c.b;
		return _out;
	}
};

class Light
{
public:
  Position position; // x, y, z position
  Color color; // R G B color

  void print()
  {
	  std::cout<<*this<<std::endl;
	 
  }

  friend std::ostream & operator<<(std::ostream & _out, const Light & light) 
  {
	   _out << "Light: " << light.position << " " << light.color;
	   return _out;
  }
};  

class Lighting
{
public:
    float totalBrightness;  
    std::vector< Light > lights;
	void print()
	{
		std::cout<<*this<<std::endl;
	}
	friend std::ostream & operator<<(std::ostream & _out, const Lighting & lighting) 
	{
		_out << "Lighting: totalBrightness=" << lighting.totalBrightness << "\n";
		for (int i=0; i< (int)lighting.lights.size(); ++i) 
		{
			_out << "#" << (i+1) << " " << lighting.lights[i] << "\n";
		}
		return _out;
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
    friend std::ostream & operator<<(std::ostream & _out, const Box & box) 
  {
	   _out << "minPosition= " << box.minPosition << ", maxPosition= "<<box.maxPosition;
	   return _out;
  }
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
  std::string imageTitle;
  bool useDefaultLighting;
  bool useDefaultCamera;
  bool useDefaultParallelogramFloor;
  bool useDefaultBox;

  // Lighting information
  Lighting lighting;
  std::vector< Box > boxes;

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
  imageTitle("GPURayTracer"),
  useDefaultLighting(true),
  useDefaultCamera(true),
  useDefaultParallelogramFloor(true),
  useDefaultBox(true)
    {
 
    }

  void print()
  {
	  std::cout<<"imageTitle = "<<imageTitle.c_str()<<std::endl;
	  std::cout<<"useDefaultLighting = "<<useDefaultLighting<<std::endl;
	  std::cout<<"useDefaultCamera = "<<useDefaultCamera<<std::endl;
	  std::cout<<"useDefaultParallelogramFloor = "<<useDefaultParallelogramFloor<<std::endl;
	  std::cout<<"useDefaultBox = "<<useDefaultBox<<std::endl;
	  if (!useDefaultLighting)
		lighting.print();
	  if (!useDefaultBox)
		  for (int i=0; i<(int)boxes.size(); ++i)
			std::cout<<"Box:"<<i+1<<boxes[i]<<std::endl;

  }

  // Since no pointers the default assignment operator, copy constructor
  // and destructors should work OK.  Not specified.
};

  

