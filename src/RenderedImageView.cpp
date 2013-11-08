#include <RenderedImageView.hpp>

#include <GLUTDisplay.hpp>

namespace grt {


	RenderedImageView::RenderedImageView(int argc, char ** argv)
	{
		GLUTDisplay::init(argc,argv);
	}

	RenderedImageView::run() {
		GLUTDisplay::run("GRT View", &scene);
	}

	RenderedImageView::modelUpdated(const Model & _model) {
		if (_model.image) {
			// Refresh display with new image
		}
	}

}; // grt
