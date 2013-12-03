#include <vector>
#include <iostream>

#include <Model.hpp>
#include "gtest/gtest.h"

class ModelTest : public testing::Test {
protected:
	grt::Model model;
	virtual void SetUp() { }
	virtual void TearDown() { }
};

class MockObserver : public grt::IModelObserver {
	protected:
		bool notified;
	public:
		MockObserver()
		: notified(false)
		{}

		bool hasBeenNotified() const {
			return notified;
		}

		void reset() {
			notified = false;
		}

		void modelUpdated(const grt::Model &) {
			notified = true;
		}
};


TEST_F(ModelTest,testObserver)
{
	MockObserver mo;

	// Ensure notification is initialized to false
	ASSERT_FALSE(mo.hasBeenNotified());

	// Register for notifications
	model.registerObserver(&mo);

	// Ensure registration did not cause a notification
	ASSERT_FALSE(mo.hasBeenNotified());

	// Cause a notification
	model.notifyObservers();

	// Ensure notified
	ASSERT_TRUE(mo.hasBeenNotified());

	// Reset Mock Observer
	mo.reset();

	// Ensure reset worked
	ASSERT_FALSE(mo.hasBeenNotified());

	// Cause another notification
	model.notifyObservers();

	// Ensure notified
	ASSERT_TRUE(mo.hasBeenNotified());

	// Cleanup
	model.unregisterObserver(&mo);

	// Reset Mock Observer
	mo.reset();

	// Ensure reset worked
	ASSERT_FALSE(mo.hasBeenNotified());

	// Cause another notification
	model.notifyObservers();

	// Ensure no notification received
	ASSERT_FALSE(mo.hasBeenNotified());
}

TEST_F(ModelTest,testMultipleObservers)
{
	MockObserver mo1;
	MockObserver mo2;
	MockObserver mo3;

	// Ensure notification is initialized to false
	ASSERT_FALSE(mo1.hasBeenNotified());
	ASSERT_FALSE(mo2.hasBeenNotified());
	ASSERT_FALSE(mo3.hasBeenNotified());

	// Register for notifications
	model.registerObserver(&mo1);
	model.registerObserver(&mo2);
	model.registerObserver(&mo3);

	// Ensure registration did not cause a notification
	ASSERT_FALSE(mo1.hasBeenNotified());
	ASSERT_FALSE(mo2.hasBeenNotified());
	ASSERT_FALSE(mo3.hasBeenNotified());

	// Cause a notification
	model.notifyObservers();

	// Ensure notified
	ASSERT_TRUE(mo1.hasBeenNotified());
	ASSERT_TRUE(mo2.hasBeenNotified());
	ASSERT_TRUE(mo3.hasBeenNotified());

	// Cleanup
	model.unregisterObserver(&mo1);
	model.unregisterObserver(&mo2);
	model.unregisterObserver(&mo3);
}
