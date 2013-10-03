#ifndef SCENE_TEST_CPP
#define SCENE_TEST_CPP

#include "gtest/gtest.h"

#include <Scene.hpp>

namespace {
    const std::string TEST_SCENE_NAME = "MySceneTestName_1";
};

class SceneTest : public testing::Test {

    public:

    protected:
        std::auto_ptr<grt::Scene> scene;

        virtual void SetUp() {
            scene.reset(new grt::Scene(TEST_SCENE_NAME));
        }

};

TEST_F(SceneTest,getName)
{
    EXPECT_EQ(TEST_SCENE_NAME,scene->getName());
}

#endif//SCENE_TEST_CPP
