#ifndef GRT_SCENE_HPP
#define GRT_SCENE_HPP

#include <string>

namespace grt {

    class Scene {
        protected:
            std::string name;

        public:
            Scene(const std::string & _name)
            : name(_name)
            {}

            // Function obtains the Scene's name
            std::string getName() const;
    };

}; // grt namespace

#endif//GRT_SCENE_HPP
