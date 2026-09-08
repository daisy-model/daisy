#include "util/mathlib.h"
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
  EXPECT_DOUBLE_EQ(pF2h(-1.0), -0.1);
  EXPECT_DOUBLE_EQ(pF2h(0.0), -1.0);
  EXPECT_DOUBLE_EQ(pF2h(1.0), -10.0);
  EXPECT_DOUBLE_EQ(pF2h(2.0), -100.0);
  EXPECT_DOUBLE_EQ(pF2h(3.0), -1000.0);
  EXPECT_NEAR(pF2h(4.2), -15848.93192461114, 1e-9);
}


TEST(MathlibTest, h2pF) {
  EXPECT_DOUBLE_EQ(h2pF(-0.1), -1.0);
  EXPECT_DOUBLE_EQ(h2pF(-1.0), 0.0);
  EXPECT_DOUBLE_EQ(h2pF(-10.0), 1.0);
  EXPECT_DOUBLE_EQ(h2pF(-100.0), 2.0);
  EXPECT_DOUBLE_EQ(h2pF(-1000.0), 3.0);
  EXPECT_NEAR(h2pF(-15848.93192461114), 4.2, 1e-12);

  for (double pF : {-1.0, 0.0, 1.0, 2.0, 3.0, 4.2}) {
    EXPECT_NEAR(h2pF(pF2h(pF)), pF, 1e-12);
  }
}

TEST(MathlibTest, rate_to_halftime) {
  EXPECT_DOUBLE_EQ(rate_to_halftime(M_LN2), 1.0);
  EXPECT_DOUBLE_EQ(rate_to_halftime(M_LN2 / 2.0), 2.0);
  EXPECT_DOUBLE_EQ(rate_to_halftime(M_LN2 / 24.0), 24.0);

  for (double rate : {M_LN2, M_LN2 / 2.0, M_LN2 / 24.0, 0.01}) {
    EXPECT_NEAR(halftime_to_rate(rate_to_halftime(rate)), rate, 1e-12);
  }
}

TEST(MathlibTest, halftime_to_rate) {
  EXPECT_DOUBLE_EQ(halftime_to_rate(1.0), M_LN2);
  EXPECT_DOUBLE_EQ(halftime_to_rate(2.0), M_LN2 / 2.0);
  EXPECT_DOUBLE_EQ(halftime_to_rate(24.0), M_LN2 / 24.0);

  for (double halftime : {1.0, 2.0, 24.0, 100.0}) {
    EXPECT_NEAR(rate_to_halftime(halftime_to_rate(halftime)), halftime, 1e-12);
  }
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
}

TEST(MathlibDeathTest, single_positive_root_of_square_equation) {
  ASSERT_DEATH(single_positive_root_of_square_equation(1, 4, 4), "assertion 'x >= 0.0' failed"); // x = -2
}

TEST(MathlibTest, single_positive_root_of_cubic_equation) {
  EXPECT_FLOAT_EQ(single_positive_root_of_cubic_equation(1, 0, 0, -27), 3);
  EXPECT_FLOAT_EQ(single_positive_root_of_cubic_equation(1, 1, 0, -12), 2);
  EXPECT_FLOAT_EQ(single_positive_root_of_cubic_equation(1, 1, 1,  -3), 1);
}
