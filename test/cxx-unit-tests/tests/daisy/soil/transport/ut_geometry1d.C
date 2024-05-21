#include <gtest/gtest.h>

#include "object_model/treelog_store.h"
#include "daisy/soil/transport/geometry1d.h"

class Geometry1DTest : public ::testing::Test {
protected:
  Geometry1D geometry;
  TreelogStore msg;
  std::vector<double> fixed;

  Geometry1DTest()
    : geometry(),
      msg(),
      fixed({-1.5, -3, -5, -7, -9, -12, -16, -20, -25, -30, -35, -40, -45, -50, -60, -70, -80, -90,
          -100,-120, -140, -160, -180, -200})
  { }

  void SetUp () override {
    bool volatile_bottom = false;
    double max_rooting_depth = -100;
    double max_interval = 10;
    geometry.initialize_zplus(volatile_bottom, fixed, max_rooting_depth, max_interval, msg);
  }

  // You can do clean-up work that doesn't throw exceptions here.
  void TearDown() override {
    // Clean up any objects or connections
  }
};

TEST_F(Geometry1DTest, Create) {
  ASSERT_GE(geometry.cell_size(), fixed.size());
  ASSERT_EQ(geometry.zminus(geometry.cell_size()), -200);
  ASSERT_EQ(geometry.bottom(), -200);
}
