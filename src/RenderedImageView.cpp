#include <RenderedImageView.hpp>
#include <GLUTDisplay.h>

namespace grt {


	RenderedImageView::RenderedImageView(int argc, char ** argv)
	{
		GLUTDisplay::init(argc,argv);
	}

	void RenderedImageView::run() {
		GLUTDisplay::run("GRT View", scene);
	}

	void RenderedImageView::modelUpdated(const Model & _model) {
		if (_model.image) {
			// Refresh display with new image
		}
	}

}; // grt
