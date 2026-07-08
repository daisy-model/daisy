#include <array>

#include <gtest/gtest.h>

#include "object_model/unit.h"

TEST(UnitTest, BaseUnitsExposeStableSymbols) {
  EXPECT_EQ(Unit::pressure(), symbol("m^-1 kg s^-2"));
  EXPECT_EQ(Unit::mass_per_volume(), symbol("m^-3 kg"));
  EXPECT_EQ(Unit::amount_of_substance_per_area_per_time(), symbol("m^-2 s^-1 mol"));
  EXPECT_EQ(Unit::energy_per_area_per_time(), symbol("kg s^-3"));
  EXPECT_EQ(Unit::mass_per_area_per_time(), symbol("m^-2 kg s^-1"));
  EXPECT_EQ(Unit::length_per_time(), symbol("m s^-1"));
}

TEST(UnitTest, BaseUnitsRemainDistinct) {
  const std::array<symbol, 6> units{
    Unit::pressure(),
    Unit::mass_per_volume(),
    Unit::amount_of_substance_per_area_per_time(),
    Unit::energy_per_area_per_time(),
    Unit::mass_per_area_per_time(),
    Unit::length_per_time(),
  };

  for (size_t i = 0; i < units.size(); ++i) {
    for (size_t j = i + 1; j < units.size(); ++j) {
      EXPECT_NE(units[i], units[j]);
    }
  }
}
