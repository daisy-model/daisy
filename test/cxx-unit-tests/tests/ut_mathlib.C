#include "mathlib.h"
#include <gtest/gtest.h>

TEST(MathlibTest, iszero) {
  EXPECT_TRUE(iszero(0));
  EXPECT_FALSE(iszero(1));
}

TEST(MathlibTest, isequal) {
  EXPECT_TRUE(isequal(1,1));
  EXPECT_FALSE(isequal(1,0.1));
}

TEST(MathlibTest, sqr) {
  EXPECT_EQ(sqr(2), 4);
  EXPECT_EQ(sqr(-3), 9);
}

TEST(MathlibTest, bound) {
  EXPECT_EQ(bound(1, 10, 2), 2);
  EXPECT_EQ(bound(1, -10, 2), 1);
  EXPECT_EQ(bound(1, 1.5, 2), 1.5);
}

TEST(MathlibTest, set_bound) {
  double a = 10;
  double b = 200;
  double x = 500;
  set_bound(a, x, b);
  EXPECT_EQ(x, 200);

  x = 0;
  set_bound(a, x, b);
  EXPECT_EQ(x, 10);
  
  x = 50.7;
  set_bound(a, x, b);
  EXPECT_EQ(x, 50.7);
}

TEST(MathlibTest, pF2h) {
  FAIL() << "Missing specification.";
}


TEST(MathlibTest, h2pF) {
  FAIL() << "Missing specification.";
}

TEST(MathlibTest, rate_to_halftime) {
  FAIL() << "Missing specification.";
}

TEST(MathlibTest, halftime_to_rate) {
  FAIL() << "Missing specification.";
}


TEST(MathlibTest, fraction_within) {
  EXPECT_EQ(fraction_within(1, 2, 0, 3), 1);
  EXPECT_EQ(fraction_within(0, 1, 2, 3), 0);
  EXPECT_EQ(fraction_within(2, 3, 0, 1), 0);
  EXPECT_EQ(fraction_within(0, 2, 1, 3), 0.5);
  EXPECT_EQ(fraction_within(0, 2, 1, 10), 0.5);
  EXPECT_EQ(fraction_within(10, 20, 14, 15), 0.1);
}

TEST(MathlibTest, single_positive_root_of_square_equation) {
  EXPECT_EQ(single_positive_root_of_square_equation(1, 1, -2), 1);
  EXPECT_EQ(single_positive_root_of_square_equation(1, 0, -9), 3);
  EXPECT_EQ(single_positive_root_of_square_equation(0, 0.25, 10), 40);
}

TEST(MathlibDeathTest, single_positive_root_of_square_equation) {
  ASSERT_DEATH(single_positive_root_of_square_equation(1, 4, 4), "assertion 'x >= 0.0' failed"); // x = -2
}

TEST(MathlibTest, single_positive_root_of_cubic_equation) {
  EXPECT_FLOAT_EQ(single_positive_root_of_cubic_equation(1, 0, 0, -27), 3);
  EXPECT_FLOAT_EQ(single_positive_root_of_cubic_equation(1, 1, 0, -12), 2);
  EXPECT_FLOAT_EQ(single_positive_root_of_cubic_equation(1, 1, 1,  -3), 1);
  EXPECT_EQ(single_positive_root_of_cubic_equation(0, 1, 0, - 9), 3);
  EXPECT_EQ(single_positive_root_of_cubic_equation(0, 0, 1, - 3), 3);
}
