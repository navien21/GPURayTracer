/*
   Includes the Model and IModelObserver class declarations.

   The Model has the items necessary to construct a modified OptiX image.

   In normal use, one or more IModelObserver objects are registered
   to the model.  Specific observers or viewers may inherit from 
   IModelObservers.

   A controller updates the model and calls the model notifyObservers
   function which calls the modelUpdated function of each of the
   observers registered to the model.

*/

#pragma once

#include <set>
#include <vector>
#include <ostream>
#include <string>

namespace grt {

	class Position
	{
		public:
			float x;
			float y;
			float z;

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

			friend std::ostream & operator<<(std::ostream & _out, const Light & light)
			{
				_out << "Light: " << light.position << " " << light.color;
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
			std::string material;
			friend std::ostream & operator<<(std::ostream & _out, const Box & box)
			{
				_out << "minPosition= " << box.minPosition << ", maxPosition= "<<box.maxPosition << ", material= " << box.material;
				return _out;
			}
	};

	// May become a shape
	class Sphere
	{
		public:
			Position position;
			float radius;
			std::string material;
			friend std::ostream & operator<<(std::ostream & _out, const Sphere & sphere)
			{
				_out << "position= " << sphere.position << ", radius= " << sphere.radius << ", material= " << sphere.material << std::endl;
				return _out;
			}
	};

	// May become a shape
	class Chull
	{
		public:
			Position position;
			float radius;
			float min;
			float max;
			int nsides;
			std::string material;
			friend std::ostream & operator<<(std::ostream & _out, const Chull & chull)
			{
				_out << "position= " << chull.position << ", radius= " << chull.radius << ", material= " << chull.material << ", min = " << chull.min << ",max = " << chull.max << std::endl;
				return _out;
			}
	};

	// May become a shape
	class Parallelogram
	{
		public:
			// how specify? vec1, vec2, anchor, Material?
	};

	class Model;

	class IModelObserver {
		public:
			virtual void modelUpdated(const Model &) = 0;
	};

	class Model {
		std::set<IModelObserver*> observers;

		public:
			std::string imageTitle;

			float totalBrightness;

			// Lights
			std::vector< Light > lights;

			// Boxes
			std::vector< Box > boxes;

			// Spheres
			std::vector< Sphere > spheres;

			// Chulls
			std::vector< Chull > chulls;

			/* Rest is not yet ready
			// Camera information
			Camera camera;

			// Parallelogram shapes (includes floor)
			std::vector< Parallelogram > parallelogram;
			*/

			// Constructor
			Model()
				: imageTitle("GPURayTracer")
			{ }

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

			friend std::ostream & operator<<(std::ostream & _out, const Model & model)
			{
				_out << "imageTitle = " << model.imageTitle << std::endl;
				_out << "totalBrightness = " << model.totalBrightness << std::endl;
				for (int i=0; i<(int)model.lights.size(); ++i)
					_out << "Light: " << (i+1) << " - " << model.lights[i] << std::endl;
				for (int i=0; i<(int)model.boxes.size(); ++i)
					_out << "Box: " << (i+1) << " - " << model.boxes[i] << std::endl;
				for (int i=0; i<(int)model.spheres.size(); ++i)
					_out << "Sphere:" << (i+1) << " - " << model.spheres[i] << std::endl;
				for (int i=0; i<(int)model.chulls.size(); ++i)
					_out << "Chull:" << (i+1) << " - " << model.chulls[i] << std::endl;
				return _out;
			}
	};

}; // grt

