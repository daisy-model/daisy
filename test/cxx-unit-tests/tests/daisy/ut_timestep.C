#include <gtest/gtest.h>

#include "daisy/timestep.h"

class TimestepTest : public ::testing::Test {
protected:
  Timestep step;

  TimestepTest()
    : step(1,2,3,4,5)
  { }
};

TEST_F(TimestepTest, ConstructionTest) {
  EXPECT_EQ(step.days(), 1);
  EXPECT_EQ(step.hours(), 2);
  EXPECT_EQ(step.minutes(), 3);
  EXPECT_EQ(step.seconds(), 4);
  EXPECT_EQ(step.microseconds(), 5);
  Timestep step0(step);
  Timestep step1 = step;
  EXPECT_TRUE(step == step0);
  EXPECT_TRUE(step == step1);
  step0 = step1;
  EXPECT_TRUE(step == step0);
}

TEST_F(TimestepTest, PrebuiltValuesTest) {
  EXPECT_EQ(Timestep::day().total_hours(), 24);
  EXPECT_EQ(Timestep::hour().total_hours(), 1);
  EXPECT_EQ(Timestep::minute().total_hours(), 1.0/60.0);
  EXPECT_EQ(Timestep::second().total_hours(), 1.0/(60.0*60.0));
  EXPECT_EQ(Timestep::microsecond().total_hours(), 1.0/(60.0*60.0*1000000.0));
  EXPECT_EQ(Timestep::zero().total_hours(), 0);
}

TEST_F(TimestepTest, OperatorTimestepTest) {
  Timestep step0 = -step;
  Timestep step1 = step + step;
  Timestep step2 = step1 / 2;
  EXPECT_EQ(step0.days(), -1);
  EXPECT_EQ(step0.hours(), -2);
  EXPECT_EQ(step0.minutes(), -3);
  EXPECT_EQ(step0.seconds(), -4);
  EXPECT_EQ(step0.microseconds(), -5);
  
  EXPECT_EQ(step1.days(), 2);
  EXPECT_EQ(step1.hours(), 4);
  EXPECT_EQ(step1.minutes(), 6);
  EXPECT_EQ(step1.seconds(), 8);
  EXPECT_EQ(step1.microseconds(), 10);
  
  EXPECT_TRUE(step == step);
  EXPECT_FALSE(step == step1);
  EXPECT_TRUE(step == step2);
}

TEST_F(TimestepTest, TotalHoursTest) {
  EXPECT_EQ(step.total_hours(), 24 + 2 + 3.0/60.0 + 4.000005/(60.0*60.0));
}

TEST_F(TimestepTest, PrintTest) {
  EXPECT_EQ(step.print(), "1d2h3m4.000005s");
}
