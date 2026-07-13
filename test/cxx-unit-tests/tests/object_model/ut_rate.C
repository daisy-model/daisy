#include <cmath>
#include <memory>
#include <set>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/block_model.h"
#include "object_model/block_top.h"
#include "object_model/frame_model.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/rate.h"
#include "object_model/treelog.h"
#include "object_model/units.h"

namespace {

void load_test_frame(Frame& frame) {
  Units::load_syntax(frame);
}

std::set<symbol> library_entries(const Library& library) {
  std::vector<symbol> entries;
  library.entries(entries);
  return std::set<symbol>(entries.begin(), entries.end());
}

std::unique_ptr<FrameModel> clone_model(const Library& library,
                                        const symbol name) {
  return std::unique_ptr<FrameModel>(&library.model(name).clone());
}

void register_test_models() {
  register_unit_models();
  register_rate_models();
}

}  // namespace

TEST(RateRegistrationTest, RateLibraryContainsExpectedModels) {
  register_test_models();
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Rate::component));
  const Library& library = metalib.library(Rate::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("rate"));
  EXPECT_TRUE(entries.count("halftime"));
  EXPECT_TRUE(entries.count("zero"));
}

TEST(RateRegistrationTest, RateModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Rate::component);

  EXPECT_TRUE(library.check("rate"));
  EXPECT_TRUE(library.check("halftime"));
  EXPECT_TRUE(library.is_derived_from("rate", "component"));
  EXPECT_TRUE(library.is_derived_from("halftime", "component"));
  EXPECT_EQ(library.base_model("rate"), symbol("component"));
  EXPECT_EQ(library.base_model("halftime"), symbol("component"));

  const FrameModel& rate_model = library.model("rate");
  const FrameModel& halftime_model = library.model("halftime");
  EXPECT_EQ(rate_model.type_name(), symbol("rate"));
  EXPECT_EQ(rate_model.base_name(), symbol("component"));
  EXPECT_EQ(halftime_model.type_name(), symbol("halftime"));
  EXPECT_EQ(halftime_model.base_name(), symbol("component"));
}

TEST(RateRegistrationTest, RateComponentSymbolIsStable) {
  EXPECT_EQ(symbol(Rate::component), symbol("rate"));
}

TEST(RateExposureTest, ConcreteRateClassesArePublicTypes) {
  EXPECT_TRUE((std::is_base_of<Rate, RateRate>::value));
  EXPECT_TRUE((std::is_base_of<Rate, RateHalftime>::value));
  EXPECT_TRUE((std::is_constructible<RateRate, double>::value));
  EXPECT_TRUE((std::is_constructible<RateHalftime, double>::value));
  EXPECT_TRUE((std::is_constructible<RateRate, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<RateHalftime, const BlockModel&>::value));
}

TEST(RateExposureTest, RateRateHasDirectRateConstructor) {
  RateRate rate(M_LN2);

  EXPECT_DOUBLE_EQ(rate.find_rate(), M_LN2);
}

TEST(RateExposureTest, RateHalftimeHasDirectHalftimeConstructor) {
  RateHalftime rate(2.0);

  EXPECT_DOUBLE_EQ(rate.find_rate(), M_LN2 / 2.0);
}

TEST(RateExposureTest, RateRateCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Rate::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "rate");
  frame->set("rate", M_LN2);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "rate");
  RateRate rate(block);

  EXPECT_DOUBLE_EQ(rate.find_rate(), M_LN2);
}

TEST(RateExposureTest, RateHalftimeCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Rate::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "halftime");
  frame->set("halftime", 2.0);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "halftime");
  RateHalftime rate(block);

  EXPECT_DOUBLE_EQ(rate.find_rate(), M_LN2 / 2.0);
}
