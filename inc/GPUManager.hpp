#ifndef GRT_GPUMANAGER_HPP
#define GRT_GPUMANAGER_HPP

namespace grt {

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

}; // grt namespace

#endif//GRT_GPUMANAGER_HPP

