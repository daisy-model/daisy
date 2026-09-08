#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/frame.h"
#include "object_model/frame_model.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/rate.h"
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

class TestFrame : public Frame {
public:
  TestFrame() = default;
  Frame& clone() const override { return *new TestFrame(*this); }
};

}  // namespace

TEST(RateRegistrationTest, RateLibraryContainsExpectedModels) {
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Rate::component));
  const Library& library = metalib.library(Rate::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("rate"));
  EXPECT_TRUE(entries.count("halftime"));
  EXPECT_TRUE(entries.count("zero"));
}

TEST(RateRegistrationTest, RateModelsExposeCurrentInheritanceMetadata) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Rate::component);

  EXPECT_TRUE(library.check("rate"));
  EXPECT_TRUE(library.check("halftime"));
  EXPECT_TRUE(library.check("zero"));

  EXPECT_TRUE(library.is_derived_from("rate", "component"));
  EXPECT_TRUE(library.is_derived_from("halftime", "component"));
  EXPECT_TRUE(library.is_derived_from("zero", "rate"));
  EXPECT_TRUE(library.is_derived_from("zero", "component"));

  EXPECT_EQ(library.base_model("rate"), symbol("component"));
  EXPECT_EQ(library.base_model("halftime"), symbol("component"));
  EXPECT_EQ(library.base_model("zero"), symbol("component"));

  const FrameModel& rate_model = library.model("rate");
  const FrameModel& halftime_model = library.model("halftime");
  const FrameModel& zero_model = library.model("zero");
  EXPECT_EQ(rate_model.type_name(), symbol("rate"));
  EXPECT_EQ(rate_model.base_name(), symbol("component"));
  EXPECT_EQ(halftime_model.type_name(), symbol("halftime"));
  EXPECT_EQ(halftime_model.base_name(), symbol("component"));
  EXPECT_EQ(zero_model.type_name(), symbol("zero"));
  EXPECT_EQ(zero_model.base_name(), symbol("rate"));
  EXPECT_DOUBLE_EQ(zero_model.number("rate"), 0.0);
}

TEST(RateHelperTest, DeclareHelpersExposeExpectedOuterFrameMetadata) {
  TestFrame frame;

  Rate::declare(frame, "decay", "Decay rate");
  Rate::declare_optional(frame, "optional_decay", "Optional decay rate");

  EXPECT_EQ(frame.lookup("decay"), Attribute::Model);
  EXPECT_EQ(frame.component("decay"), symbol(Rate::component));
  EXPECT_EQ(frame.type_size("decay"), Attribute::Singleton);
  EXPECT_EQ(frame.description("decay"), symbol("Decay rate"));
  EXPECT_TRUE(frame.is_state("decay"));
  EXPECT_FALSE(frame.is_optional("decay"));
  EXPECT_FALSE(frame.is_const("decay"));

  EXPECT_EQ(frame.lookup("optional_decay"), Attribute::Model);
  EXPECT_EQ(frame.component("optional_decay"), symbol(Rate::component));
  EXPECT_TRUE(frame.is_optional("optional_decay"));
  EXPECT_TRUE(frame.is_const("optional_decay"));
}

TEST(RateHelperTest, SetHelpersPopulateNestedRateModels) {
  TestFrame frame;
  Rate::declare(frame, "direct", "Direct rate");
  Rate::declare(frame, "half", "Halftime rate");
  Rate::declare(frame, "cited_half", "Cited halftime rate");

  Rate::set_rate(frame, "direct", 0.25);
  Rate::set_halftime(frame, "half", 8.0);
  Rate::set_halftime_cited(frame, "cited_half", 12.0, "Measured halftime",
                           "ref-1");

  ASSERT_TRUE(frame.check("direct"));
  ASSERT_TRUE(frame.check("half"));
  ASSERT_TRUE(frame.check("cited_half"));

  const FrameModel& direct_model = frame.model("direct");
  EXPECT_EQ(direct_model.type_name(), symbol("rate"));
  EXPECT_DOUBLE_EQ(direct_model.number("rate"), 0.25);

  const FrameModel& half_model = frame.model("half");
  EXPECT_EQ(half_model.type_name(), symbol("halftime"));
  EXPECT_DOUBLE_EQ(half_model.number("halftime"), 8.0);

  const FrameModel& cited_model = frame.model("cited_half");
  EXPECT_EQ(cited_model.type_name(), symbol("halftime"));
  EXPECT_DOUBLE_EQ(cited_model.number("halftime"), 12.0);
  EXPECT_EQ(cited_model.value_description("halftime"), symbol("Measured halftime"));
  ASSERT_EQ(cited_model.value_cite("halftime").size(), 1U);
  EXPECT_EQ(cited_model.value_cite("halftime")[0], symbol("ref-1"));
}

TEST(RateRegistrationTest, ComponentSymbolIsStable) {
  EXPECT_EQ(symbol(Rate::component), symbol("rate"));
}
