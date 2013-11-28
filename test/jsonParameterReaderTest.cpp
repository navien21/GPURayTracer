#include <vector>
#include <iostream>

#include "jsonParameterReader.h"
#include "RayTraceImageData.h"
#include "gtest/gtest.h"

class jsonParameterReaderTest : public testing::Test {
protected:
	RayTraceImageData resultData;
	virtual void SetUp() {
		LoadRayTraceImageFromJsonFile("params/fr1.json", resultData);
	}
};



TEST_F(jsonParameterReaderTest,testLightColor) {
			
	//We expect just one in this scene.
	ASSERT_EQ(1,resultData.lighting.lights.size());


	//RGB Values should all be 1
	ASSERT_EQ(1, resultData.lighting.lights[0].color.r);
	ASSERT_EQ(1, resultData.lighting.lights[0].color.g);
	ASSERT_EQ(1, resultData.lighting.lights[0].color.b);
}

TEST_F(jsonParameterReaderTest,testLightPos) {
	

	//Same scene, so still one light.  Retest just to be sure.
	ASSERT_EQ(1,resultData.lighting.lights.size());

	ASSERT_EQ(-5.0, resultData.lighting.lights[0].position.x);
	ASSERT_EQ(60.0, resultData.lighting.lights[0].position.y);
	ASSERT_EQ(-16.0, resultData.lighting.lights[0].position.z);

}