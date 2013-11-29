#include <vector>
#include <iostream>

#include <Model.hpp>
#include <ParseJSONController.hpp>
#include "gtest/gtest.h"

class ParseJSONControllerTest : public testing::Test {
protected:
	grt::Model resultData;
	grt::ParseJSONController * pjc;
	virtual void SetUp() {
		pjc = new grt::ParseJSONController(resultData,"params/fr1.json");
		pjc->execute();
	}

	virtual void TearDown() {
		delete pjc;
	}
};


TEST_F(ParseJSONControllerTest,testLightColor)
{
	//We expect just one in this scene.
	ASSERT_EQ(1,resultData.lights.size());

	//RGB Values should all be 1
	ASSERT_EQ(1, resultData.lights[0].color.r);
	ASSERT_EQ(1, resultData.lights[0].color.g);
	ASSERT_EQ(1, resultData.lights[0].color.b);
}

TEST_F(ParseJSONControllerTest,testLightPos)
{
	//Same scene, so still one light.  Retest just to be sure.
	ASSERT_EQ(1,resultData.lights.size());

	ASSERT_EQ( -5.0, resultData.lights[0].position.x);
	ASSERT_EQ( 60.0, resultData.lights[0].position.y);
	ASSERT_EQ(-16.0, resultData.lights[0].position.z);
}
