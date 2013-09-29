#ifndef GRT_GPUMANAGER_HPP
#define GRT_GPUMANAGER_HPP

namespace gtr {

    // Forward declarations
    class Scene;
    class Image;

    class GPUManager
    {
        public:
            void upload(const Scene &);
            void render();
            void download(Image &);
    };

}; // gtr namespace

#endif//GRT_GPUMANAGER_HPP

