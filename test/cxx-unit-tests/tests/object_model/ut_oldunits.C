#include <gtest/gtest.h>

#include "object_model/attribute.h"
#include "object_model/oldunits.h"

TEST(OldunitsTest, ExposesStableCommonUnitSymbols) {
  EXPECT_EQ(Oldunits::h(), symbol("h"));
  EXPECT_EQ(Oldunits::mm(), symbol("mm"));
  EXPECT_EQ(Oldunits::per_mm(), symbol("mm^-1"));
  EXPECT_EQ(Oldunits::mm_per_h(), symbol("mm/h"));
  EXPECT_EQ(Oldunits::cm(), symbol("cm"));
  EXPECT_EQ(Oldunits::cm_per_h(), symbol("cm/h"));
  EXPECT_EQ(Oldunits::cm2(), symbol("cm^2"));
  EXPECT_EQ(Oldunits::cm3(), symbol("cm^3"));
  EXPECT_EQ(Oldunits::per_h(), symbol("h^-1"));
  EXPECT_EQ(Oldunits::ppm(), symbol("ppm"));
}

TEST(OldunitsTest, ConvertsKnownUnitsAndPreservesIdentityConversions) {
  EXPECT_TRUE(Oldunits::can_convert("mm/h", "cm/h"));
  EXPECT_DOUBLE_EQ(Oldunits::convert("mm/h", "cm/h", 12.0), 1.2);

  EXPECT_TRUE(Oldunits::can_convert("mm/d", "cm/h"));
  EXPECT_DOUBLE_EQ(Oldunits::convert("mm/d", "cm/h", 24.0), 0.1);

  EXPECT_DOUBLE_EQ(Oldunits::convert("cm", "cm", 3.5), 3.5);
}

TEST(OldunitsTest, RejectsInvalidValuesForGuardedConversions) {
  EXPECT_TRUE(Oldunits::can_convert("cm", "pF", -100.0));
  EXPECT_FALSE(Oldunits::can_convert("cm", "pF", 10.0));
  EXPECT_DOUBLE_EQ(Oldunits::convert("cm", "pF", -100.0), 2.0);
  EXPECT_ANY_THROW(Oldunits::convert("cm", "pF", 10.0));

  EXPECT_TRUE(Oldunits::can_convert("pF", "cm", 2.0));
  EXPECT_DOUBLE_EQ(Oldunits::convert("pF", "cm", 2.0), -100.0);
}

TEST(OldunitsTest, ReportsUnavailableConversions) {
  EXPECT_FALSE(Oldunits::can_convert("unknown", "cm"));
  EXPECT_FALSE(Oldunits::can_convert("cm", "unknown"));
}

TEST(OldunitsTest, MultiplyHandlesNeutralKnownAndInverseCases) {
  EXPECT_EQ(Oldunits::multiply(Attribute::None(), "cm"), symbol("cm"));
  EXPECT_EQ(Oldunits::multiply(Attribute::Fraction(), "cm"), symbol("cm"));
  EXPECT_EQ(Oldunits::multiply("", "cm"), symbol("cm"));

  EXPECT_EQ(Oldunits::multiply("h^-1", "cm"), symbol("cm/h"));
  EXPECT_EQ(Oldunits::multiply("cm", "h^-1"), symbol("cm/h"));
  EXPECT_EQ(Oldunits::multiply("cm/h", "h"), symbol("cm"));
  EXPECT_EQ(Oldunits::multiply("cm", "cm^-1"), Attribute::None());
  EXPECT_EQ(Oldunits::multiply(Attribute::Unknown(), "cm"), Attribute::Unknown());
  EXPECT_EQ(Oldunits::multiply("foo", "bar"), Attribute::Unknown());
}
