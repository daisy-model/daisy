#include <gtest/gtest.h>

#include "object_model/check.h"
#include "object_model/treelog_text.h"

namespace {

void expect_valid(const Check& check, const double value) {
  TreelogString log;
  EXPECT_TRUE(check.verify(value, log));
  EXPECT_TRUE(log.str().empty());
}

void expect_invalid(const Check& check, const double value) {
  TreelogString log;
  EXPECT_FALSE(check.verify(value, log));
  EXPECT_FALSE(log.str().empty());
}

}  // namespace

TEST(CheckTest, NoneAndUnknownAcceptAllValues) {
  expect_valid(Check::none(), -42.0);
  expect_valid(Check::unknown(), 0.0);
}

TEST(CheckTest, NonZeroRejectsOnlyNonNormalValues) {
  expect_valid(Check::non_zero(), 1.0);
  expect_valid(Check::non_zero(), -0.5);
  expect_invalid(Check::non_zero(), 0.0);
}

TEST(CheckTest, NonNegativeHonorsBoundary) {
  expect_valid(Check::non_negative(), 0.0);
  expect_valid(Check::non_negative(), 0.25);
  expect_invalid(Check::non_negative(), -0.25);
}

TEST(CheckTest, NonPositiveHonorsBoundary) {
  expect_valid(Check::non_positive(), 0.0);
  expect_valid(Check::non_positive(), -0.25);
  expect_invalid(Check::non_positive(), 0.25);
}

TEST(CheckTest, NegativeRequiresStrictlyNegativeValues) {
  expect_valid(Check::negative(), -0.25);
  expect_invalid(Check::negative(), 0.0);
  expect_invalid(Check::negative(), 0.25);
}

TEST(CheckTest, PositiveRequiresStrictlyPositiveValues) {
  expect_valid(Check::positive(), 0.25);
  expect_invalid(Check::positive(), 0.0);
  expect_invalid(Check::positive(), -0.25);
}

TEST(CheckTest, FractionRequiresClosedZeroOneInterval) {
  expect_valid(Check::fraction(), 0.0);
  expect_valid(Check::fraction(), 1.0);
  expect_valid(Check::fraction(), 0.5);
  expect_invalid(Check::fraction(), -0.1);
  expect_invalid(Check::fraction(), 1.1);
}

TEST(CheckTest, DSGuardUsesConfiguredRange) {
  expect_valid(Check::DS(), -1.0);
  expect_valid(Check::DS(), 2.0);
  expect_valid(Check::DS(), 0.75);
  expect_invalid(Check::DS(), -1.1);
  expect_invalid(Check::DS(), 2.1);
}
