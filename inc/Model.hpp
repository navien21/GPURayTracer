/*
 * Includes items necessary to construct an image or scene.
*/

#pragma once

#include <vector>
#include <set>
#include <ostream>

namespace grt {

	class Position
	{
		public:
			float x;
			float y;
			float z;

			friend std::ostream & operator<<(std::ostream & _out, const Position & pos) {
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

			friend std::ostream & operator<<(std::ostream & _out, const Color & c) {
				_out << "color: r=" << c.r << ", g=" << c.g << ", b= " << c.b;
				return _out;
			}
	};

	class Light
	{
		public:
			Position position; // x, y, z position
			Color color; // R G B color

			friend std::ostream & operator<<(std::ostream & _out, const Light & light) {
				_out << "Light: " << light.position << " " << light.color;
				return _out;
			}
	};

	class Lighting
	{
		public:
			float totalBrightness;
			std::vector< Light > lights;

			friend std::ostream & operator<<(std::ostream & _out, const Lighting & lighting) {
				_out << "Lighting: totalBrightness=" << lighting.totalBrightness << std::endl;
				for (int i=0; i< lighting.lights.size(); ++i) {
					_out << "#" << (i+1) << " " << lighting.lights[i] << std::endl;
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
		// Material would go here? Or maybe several colors?
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

		protected:
			std::set<IModelObserver*> observers;

		public:

			Camera camera;
			Lighting lighting;
			void * image;

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

}; // grt
