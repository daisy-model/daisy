#include <gtest/gtest.h>

#include "metalib.h"
#include "block_top.h"
#include "treelog_store.h"
#include "geometry1d.h"

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

// TEST_F(Geometry1DTest, ZMinusTest) {
//     // Test the zminus() function
//     ASSERT_DOUBLE_EQ(geometry.zminus(0), 0.0);
//     ASSERT_DOUBLE_EQ(geometry.zminus(1), geometry.zplus(0)); // Assuming zplus() is correctly implemented
//     // Add more test cases as needed
// }

// TEST_F(Geometry1DTest, CellVolumeTest) {
//     ASSERT_DOUBLE_EQ(geometry.cell_volume(0), 0.0);
// }

// TEST_F(Geometry1DTest, EdgeSizeTest) {
//     ASSERT_EQ(geometry.edge_size(), geometry.cell_size() + 1);
// }

// TEST_F(Geometry1DTest, EdgeNameTest) {
//     ASSERT_EQ(geometry.edge_name(0), "0");
// }

// TEST_F(Geometry1DTest, EdgeFromTest) {
//     ASSERT_EQ(geometry.edge_from(0), geometry.cell_below);
//     ASSERT_EQ(geometry.edge_from(1), 0); // Assuming valid cell indices
// }

// TEST_F(Geometry1DTest, EdgeToTest) {
//     ASSERT_EQ(geometry.edge_to(0), 0); // Assuming valid cell indices
//     ASSERT_EQ(geometry.edge_to(1), 1);
// }

// TEST_F(Geometry1DTest, EdgeCenterZTest) {
//     ASSERT_DOUBLE_EQ(geometry.edge_center_z(0), 0.0);
//     ASSERT_DOUBLE_EQ(geometry.edge_center_z(1), geometry.zplus(0));
// }

// TEST_F(Geometry1DTest, AddSoilTest) {
//     std::vector<double> soilVector; // Initialize as needed
//     double top = 0.0; // Example top value
//     double bottom = 10.0; // Example bottom value
//     double left = 5.0; // Example left value
//     double right = 15.0; // Example right value
//     double amount = 2.5; // Example amount

//     geometry.add_soil(soilVector, top, bottom, left, right, amount);

//     // Validate the modified soilVector
//     // Add more assertions as needed
// }
