#pragma once

#include <Model.hpp>
#include <SampleScene.h>

namespace grt {

class Scene : public SampleScene {
	Scene()
	: SampleScene()
	{
		

	}
};

class RenderedImageView
: public IModelObserver
{

	protected:
		Scene *scene;

	public:
		RenderedImageView(int,char**);

		void run();

		void modelUpdated(const Model & _model);

};

};
