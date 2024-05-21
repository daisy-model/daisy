#include <gtest/gtest.h>

#include "object_model/treelog_store.h"
#include "daisy/soil/transport/geometry_rect.h"

class GeometryRectTest : public ::testing::Test {
protected:
  std::vector<double> zplus;
  std::vector<double> xplus;
  GeometryRect geometry;
  TreelogStore msg;


  GeometryRectTest()
    : zplus({-1.5, -3, -5, -7, -9, -12, -16, -20, -25, -30, -35, -40, -45, -50, -60, -70, -80, -90,
        -100,-120, -140, -160, -180, -200}),
      xplus({15, 30}),
      geometry(zplus, xplus)
  { }

  void SetUp () override {
  }

  void TearDown() override {
  }
};

TEST_F(GeometryRectTest, Create) {
  ASSERT_GE(geometry.cell_rows(), zplus.size());
  ASSERT_GE(geometry.cell_columns(), xplus.size());
  ASSERT_EQ(geometry.bottom(), zplus.back());
  ASSERT_EQ(geometry.right(), xplus.back());
}
