
#include <Scene.hpp>
#include <Image.hpp>
#include <GPUManager.hpp>

namespace {
    void parseArgs(int,char**) {}
};

int main(int argc, char ** argv)
{

    // Parse User params
    parseArgs(argc,argv);

    // Initialize GPU
    gtr::GPUManager gman;

    // Construct Scene
    gtr::Scene scene;

    // Offload to GPU
    gman.upload(scene);

    // GPU Processing
    gman.render();

    // Obtain image result from GPU
    gtr::Image img;
    gman.download(img);

    // Output image
    img.save();

    // Cleanup

    return 0;
}
