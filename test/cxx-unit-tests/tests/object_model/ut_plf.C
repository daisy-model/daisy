#include <cmath>
#include <gtest/gtest.h>
#include "object_model/plf.h" 

class PLFTest : public ::testing::Test {
protected:
  PLF y_is_2x_neg5_to_5;
  PLF y_is_x_0_to_3;
  PLF sine_wave;
  
  void SetUp() override {
    for (int x = -5; x <= 5; ++x) {
      y_is_2x_neg5_to_5.add(x, 2*x);
    }
    y_is_x_0_to_3.add(0.0, 0.0);
    y_is_x_0_to_3.add(1.0, 1.0);
    y_is_x_0_to_3.add(2.0, 2.0);
    y_is_x_0_to_3.add(3.0, 3.0);
    
    constexpr int numPoints = 1000;
    constexpr double step = 2*M_PI/999;
    for (int i = 0; i < numPoints; ++i) {
      double x = i * step;
      double y = sin(x);
      sine_wave.add(x, y);
    }
  }

  // You can do clean-up work that doesn't throw exceptions here.
  void TearDown() override {
    // Clean up any objects or connections
  }
};

// Test case for testing the PLF class functionality
TEST_F(PLFTest, FunctionalityTest) {
  PLF plf;

  // Test default constructor
  EXPECT_EQ(plf.size(), 0);

  // Test add() method
  plf.add(1.0, 2.0);
  plf.add(2.0, 3.0);
  EXPECT_EQ(plf.size(), 2);

  // Test x() and y() methods
  EXPECT_DOUBLE_EQ(plf.x(0), 1.0);
  EXPECT_DOUBLE_EQ(plf.y(0), 2.0);
  EXPECT_DOUBLE_EQ(plf.x(1), 2.0);
  EXPECT_DOUBLE_EQ(plf.y(1), 3.0);

  // Test clear() method
  plf.clear();
  EXPECT_EQ(plf.size(), 0);

  // Test size() method
  EXPECT_EQ(sine_wave.size(), 1000);

  // Test some specific points to ensure sine wave data is correctly added
  EXPECT_DOUBLE_EQ(sine_wave.x(0), 0.0);
  EXPECT_DOUBLE_EQ(sine_wave.y(0), 0.0);
  EXPECT_DOUBLE_EQ(sine_wave.x(99), 2*M_PI*99/999);
  EXPECT_DOUBLE_EQ(sine_wave.y(99), sin(2*M_PI*99/999));
  EXPECT_DOUBLE_EQ(sine_wave.x(999), 2*M_PI);
  EXPECT_NEAR(sine_wave.y(999), 0.0, 1e-15);
}

// Test case for testing comparison operators
TEST_F(PLFTest, ComparisonTest) {
  PLF plf1, plf2, plf3;
  plf1.add(1.0, 2.0);
  plf2.add(1.0, 2.0);
  plf3.add(0.0, 0.0);
  plf3.add(3.0, 1.5);
  // Test operator==
  EXPECT_TRUE(plf1 == plf2);

  // Test operator<
  EXPECT_FALSE(plf1 < plf2);
  EXPECT_TRUE(plf3 < plf1);
  EXPECT_FALSE(plf2 < plf3);

  // Test operator<=
  EXPECT_TRUE(plf1 <= plf2);
  EXPECT_TRUE(plf3 <= plf1);
  EXPECT_FALSE(plf2 <= plf3);
}


// Test case for testing the integrate() method
TEST_F(PLFTest, IntegrateTest) {
  // Test integrate() method for different intervals
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.integrate(0.0, 1.0), 0.5); 
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.integrate(1.0, 2.0), 1.5); 
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.integrate(2.0, 3.0), 2.5);
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.integrate(0.0, 3.0), 4.5);
}


// Test case for testing the offset()
TEST_F(PLFTest, OffsetTest) {
  // Test offset() method
  y_is_x_0_to_3.offset(1.0);  // Offset all y values by 1
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.y(0), 1.0);
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.y(1), 2.0);
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.y(2), 3.0);
  EXPECT_DOUBLE_EQ(y_is_x_0_to_3.y(3), 4.0);
}

// Test case for testing the inverse() method
TEST_F(PLFTest, InverseTest) {
  
  // Test inverse() method
  PLF inverse1 = y_is_x_0_to_3.inverse();
  EXPECT_DOUBLE_EQ(inverse1(0.0), y_is_x_0_to_3(0.0));
  EXPECT_DOUBLE_EQ(inverse1(1.0), y_is_x_0_to_3(1.0));
  EXPECT_DOUBLE_EQ(inverse1(2.0), y_is_x_0_to_3(2.0));
  EXPECT_DOUBLE_EQ(inverse1(3.0), y_is_x_0_to_3(3.0));

  // Test inverse() method
  PLF inverse2 = y_is_2x_neg5_to_5.inverse();
  EXPECT_DOUBLE_EQ(inverse2(-8.4), -4.2);
  EXPECT_DOUBLE_EQ(inverse2(0.0), 0.0);
  EXPECT_DOUBLE_EQ(inverse2(3.5), 1.75);
}

// Test case for testing the operator+= method
TEST_F(PLFTest, OperatorPlusEqualTest) {
  PLF plf1, plf2;

  // Add some sample data to both PLFs
  plf1.add(0.0, 0.0);
  plf1.add(1.0, 1.0);
  plf2.add(0.5, 0.5);
  plf2.add(1.5, 1.5);

  // Perform addition using operator+=
  plf1 += plf2;

  // Test the size after addition
  EXPECT_EQ(plf1.size(), 4);

  // Test some specific points after addition
  EXPECT_DOUBLE_EQ(plf1.x(0), 0.0);
  EXPECT_DOUBLE_EQ(plf1.y(0), 0.5);
  EXPECT_DOUBLE_EQ(plf1.x(1), 0.5);
  EXPECT_DOUBLE_EQ(plf1.y(1), 1.0);
  EXPECT_DOUBLE_EQ(plf1.x(2), 1.0);
  EXPECT_DOUBLE_EQ(plf1.y(2), 2.0);
  EXPECT_DOUBLE_EQ(plf1.x(3), 1.5);
  EXPECT_DOUBLE_EQ(plf1.y(3), 2.5);
}

// Test case for testing the operator() method
TEST_F(PLFTest, OperatorParenthesesTest) {
  // Test some specific points on a sine wave
  EXPECT_NEAR(sine_wave(0.0), 0.0, 1e-10);              // Sine of 0 is 0
  EXPECT_NEAR(sine_wave(M_PI / 2.0), 1.0, 1e-5);        // Sine of pi/2 is 1
  EXPECT_NEAR(sine_wave(M_PI), 0.0, 1e-10);              // Sine of pi is 0
  EXPECT_NEAR(sine_wave(3 * M_PI / 2.0), -1.0, 1e-5);   // Sine of 3pi/2 is -1 
  EXPECT_NEAR(sine_wave(2 * M_PI), 0.0, 1e-10);          // Sine of 2pi is 0
}


// Test case for testing the min() method
TEST_F(PLFTest, MinTest) {
  PLF plf;
  // Add some sample data
  plf.add(0.0, 0.0);
  plf.add(1.0, 1.0);
  plf.add(2.0, -1.0);
  plf.add(3.0, 0.5);

  // Test min() method
  EXPECT_DOUBLE_EQ(plf.min(), -1.0);  // Minimum value in the data is -1.0
}

// Test case for testing the max() method
TEST_F(PLFTest, MaxTest) {
  PLF plf;
  // Add some sample data
  plf.add(0.0, 0.0);
  plf.add(1.0, 1.0);
  plf.add(2.0, -1.0);
  plf.add(3.0, 0.5);

  // Test max() method
  EXPECT_DOUBLE_EQ(plf.max(), 1.0);  // Maximum value in the data is 1.0
}

// Test case for testing the max_at() method
TEST_F(PLFTest, MaxAtTest) {
  PLF plf;
  // Add some sample data
  plf.add(0.0, 0.0);
  plf.add(1.0, 1.0);
  plf.add(2.0, -1.0);
  plf.add(3.0, 0.5);

  // Test max_at() method
  EXPECT_DOUBLE_EQ(plf.max_at(), 1.0);  // Maximum value occurs at x = 1.0
}

// Test case for testing the first_interesting() method
TEST_F(PLFTest, FirstInterestingTest) {
  PLF plf;
  // Add some sample data
  plf.add(0.0, 0.0);
  plf.add(1.0, 0.0);
  plf.add(2.0, 0.0);
  plf.add(3.0, 1.0);

  // Test first_interesting() method
  EXPECT_DOUBLE_EQ(plf.first_interesting(), 2.0);  
}

// Test case for testing the last_interesting() method
TEST_F(PLFTest, LastInterestingTest) {
  PLF plf;
  // Add some sample data
  plf.add(0.0, 1.0);
  plf.add(1.0, 0.0);
  plf.add(2.0, 0.0);
  plf.add(3.0, 0.0);

  // Test last_interesting() method
  EXPECT_DOUBLE_EQ(plf.last_interesting(), 1.0); 
}

// Test case for testing the inverse_safe() method
TEST_F(PLFTest, InverseSafeTest) {
  
  // Test inverse() method
  PLF inverse = sine_wave.inverse_safe();
  EXPECT_DOUBLE_EQ(inverse.min(), 0.0);
  double step_length = 2*M_PI/999;
  int n_steps = round((M_PI/2)/step_length);
  EXPECT_DOUBLE_EQ(inverse.max(), n_steps * step_length);
  EXPECT_NEAR(inverse.max_at(), 1.0, 1e-5);
}
