// ut_units.C -- units unit tests 

#include "object_model/units.h"
#include "object_model/convert.h"
#include "object_model/unit.h"
#include "object_model/metalib.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/treelog.h"
#include "object_model/treelog_text.h"
#include "util/assertion.h"
#include "util/mathlib.h"

#include <gtest/gtest.h>

struct UnitsTest : public testing::Test
{
  const Assertion::Register shut_up;
  const std::unique_ptr<Metalib> metalib;
  const Units& units;

  static std::unique_ptr<Metalib> make_metalib ()
  {
    register_unit_models ();
    return std::make_unique<Metalib> (Units::load_syntax);
  }
  
  UnitsTest ()
    : shut_up (Treelog::null ()),
      metalib (make_metalib ()),
      units (metalib->units ())
  { }
};
  
TEST_F (UnitsTest, MassAreaFlux2HeightFlux)
{
  EXPECT_TRUE (units.has_unit (Units::mm_per_h ()));
  EXPECT_TRUE (units.has_unit (Unit::mass_per_area_per_time ()));
  ASSERT_TRUE (units.can_convert (Unit::mass_per_area_per_time (),
                                  Units::mm_per_h ()));
  EXPECT_DOUBLE_EQ (units.convert (Unit::mass_per_area_per_time (),
                                   Units::mm_per_h (),
                                   1.0),
                    3600.0);
  const symbol svat_ssoc_transp ("kg m^-2 s^-1");
  EXPECT_TRUE (units.has_unit (svat_ssoc_transp));
  ASSERT_TRUE (units.can_convert (svat_ssoc_transp,
                                  Units::mm_per_h ()));
  EXPECT_DOUBLE_EQ (units.convert (svat_ssoc_transp, Units::mm_per_h (), 1.0),
                    3600.0);
}

TEST_F (UnitsTest, Kelvin2Celcius)
{
  EXPECT_TRUE (units.has_unit ("K"));
  EXPECT_TRUE (units.has_unit ("dg C"));
  ASSERT_TRUE (units.can_convert ("K", "dg C"));
  EXPECT_NEAR (units.convert ("K", "dg C", 0.0), -273.15, 0.01);
}

TEST_F (UnitsTest, Kelvin2Fahrenheit)
{
  EXPECT_TRUE (units.has_unit ("K"));
  EXPECT_TRUE (units.has_unit ("dg F"));
  ASSERT_TRUE (units.can_convert ("K", "dg F"));
  EXPECT_NEAR (units.convert ("K", "dg F", 0.0), -459.67, 0.01);
}

TEST_F (UnitsTest, Radians2Degrees)
{
  EXPECT_TRUE (units.has_unit ("rad"));
  EXPECT_TRUE (units.has_unit ("dg"));
  ASSERT_TRUE (units.can_convert ("rad", "dg"));
  EXPECT_NEAR (units.convert ("rad", "dg", 0.0), 0.0, 0.01);
  EXPECT_NEAR (units.convert ("rad", "dg", M_PI), 180.0, 0.01);
  EXPECT_NEAR (units.convert ("rad", "dg", -M_PI), -180.0, 0.01);
  EXPECT_NEAR (units.convert ("dg", "rad", 0.0), 0.0, 0.01);
  EXPECT_NEAR (units.convert ("dg", "rad", 180.0), M_PI, 0.0001);
  EXPECT_NEAR (units.convert ("dg", "rad", -180.0), -M_PI, 0.0001);
}

TEST_F(UnitsTest, SupportsOldUnitFallbackAndReportsUnsupportedConversions) {
  TreelogString log;

  EXPECT_TRUE(units.can_convert("cm", "pF"));
  EXPECT_DOUBLE_EQ(units.convert("cm", "pF", -100.0), 2.0);

  EXPECT_FALSE(units.can_convert("banana", "cm", log));
  EXPECT_FALSE(log.str().empty());
}

TEST_F(UnitsTest, GetConvertionProvidesStableIdentityAndOldConverters) {
  const Convert& identity = units.get_convertion("K", "K");
  EXPECT_TRUE(identity.valid(-273.15));
  EXPECT_DOUBLE_EQ(identity(3.25), 3.25);

  const Convert& old_convert = units.get_convertion("cm", "pF");
  const Convert& old_convert_again = units.get_convertion("cm", "pF");
  EXPECT_EQ(&old_convert, &old_convert_again);
  EXPECT_TRUE(old_convert.valid(-100.0));
  EXPECT_FALSE(old_convert.valid(10.0));
  EXPECT_DOUBLE_EQ(old_convert(-100.0), 2.0);
}

// ut_units.C ends here.
