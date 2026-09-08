#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/frame_model.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/parameter_types/integer.h"
#include "object_model/parameter_types/number.h"
#include "object_model/parameter_types/stringer.h"
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

}  // namespace

TEST(NumberRegistrationTest, NumberLibraryContainsExpectedModels) {
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Number::component));
  const Library& library = metalib.library(Number::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("const"));
  EXPECT_TRUE(entries.count("x"));
  EXPECT_TRUE(entries.count("get"));
  EXPECT_TRUE(entries.count("fetch"));
  EXPECT_TRUE(entries.count("child"));
  EXPECT_TRUE(entries.count("identity"));
  EXPECT_TRUE(entries.count("convert"));
  EXPECT_TRUE(entries.count("dim"));
  EXPECT_TRUE(entries.count("plf"));
  EXPECT_TRUE(entries.count("apply"));
  EXPECT_TRUE(entries.count("if"));
  EXPECT_TRUE(entries.count("let"));
  EXPECT_TRUE(entries.count("sqrt"));
  EXPECT_TRUE(entries.count("pow"));
  EXPECT_TRUE(entries.count("max"));
  EXPECT_TRUE(entries.count("/"));
}

TEST(NumberRegistrationTest, NumberLibraryExposesExpectedInheritance) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);

  EXPECT_TRUE(library.is_derived_from("const", "component"));
  EXPECT_TRUE(library.is_derived_from("plf", "component"));
  EXPECT_TRUE(library.is_derived_from("child", "component"));
  EXPECT_TRUE(library.is_derived_from("identity", "child"));
  EXPECT_TRUE(library.is_derived_from("convert", "child"));
  EXPECT_TRUE(library.is_derived_from("dim", "child"));
  EXPECT_TRUE(library.is_derived_from("identity", "component"));
  EXPECT_TRUE(library.is_derived_from("convert", "component"));

  EXPECT_EQ(library.base_model("const"), symbol("component"));
  EXPECT_EQ(library.base_model("plf"), symbol("component"));
  EXPECT_EQ(library.base_model("child"), symbol("component"));
  EXPECT_EQ(library.base_model("identity"), symbol("component"));
  EXPECT_EQ(library.base_model("convert"), symbol("component"));
  EXPECT_EQ(library.base_model("dim"), symbol("component"));

  const FrameModel& identity_model = library.model("identity");
  const FrameModel& convert_model = library.model("convert");
  EXPECT_EQ(identity_model.type_name(), symbol("identity"));
  EXPECT_EQ(identity_model.base_name(), symbol("child"));
  EXPECT_EQ(convert_model.type_name(), symbol("convert"));
  EXPECT_EQ(convert_model.base_name(), symbol("child"));
}

TEST(IntegerRegistrationTest, IntegerLibraryContainsExpectedModels) {
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Integer::component));
  const Library& library = metalib.library(Integer::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("const"));
  EXPECT_TRUE(entries.count("cond"));
  EXPECT_TRUE(entries.count("sqr"));
  EXPECT_TRUE(entries.count("max"));
  EXPECT_TRUE(entries.count("min"));
  EXPECT_TRUE(entries.count("*"));
  EXPECT_TRUE(entries.count("+"));
  EXPECT_TRUE(entries.count("-"));
  EXPECT_TRUE(entries.count("mod"));
  EXPECT_TRUE(entries.count("div"));
}

TEST(IntegerRegistrationTest, IntegerModelsInheritFromComponentBase) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Integer::component);

  EXPECT_TRUE(library.is_derived_from("const", "component"));
  EXPECT_TRUE(library.is_derived_from("cond", "component"));
  EXPECT_TRUE(library.is_derived_from("div", "component"));
  EXPECT_EQ(library.base_model("const"), symbol("component"));
  EXPECT_EQ(library.base_model("cond"), symbol("component"));
  EXPECT_EQ(library.base_model("div"), symbol("component"));

  const FrameModel& const_model = library.model("const");
  const FrameModel& cond_model = library.model("cond");
  EXPECT_EQ(const_model.type_name(), symbol("const"));
  EXPECT_EQ(const_model.base_name(), symbol("component"));
  EXPECT_EQ(cond_model.type_name(), symbol("cond"));
  EXPECT_EQ(cond_model.base_name(), symbol("component"));
}

TEST(StringRegistrationTest, StringLibraryContainsExpectedModels) {
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Stringer::component));
  const Library& library = metalib.library(Stringer::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("cond"));
  EXPECT_TRUE(entries.count("number"));
  EXPECT_TRUE(entries.count("value"));
  EXPECT_TRUE(entries.count("dimension"));
  EXPECT_TRUE(entries.count("identity"));
}

TEST(StringRegistrationTest, StringLibraryTracksNumberDerivedHelpers) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Stringer::component);

  EXPECT_TRUE(library.is_derived_from("number", "component"));
  EXPECT_TRUE(library.is_derived_from("value", "number"));
  EXPECT_TRUE(library.is_derived_from("dimension", "number"));
  EXPECT_TRUE(library.is_derived_from("identity", "component"));

  EXPECT_EQ(library.base_model("number"), symbol("component"));
  EXPECT_EQ(library.base_model("value"), symbol("component"));
  EXPECT_EQ(library.base_model("dimension"), symbol("component"));
  EXPECT_EQ(library.base_model("identity"), symbol("component"));

  const FrameModel& value_model = library.model("value");
  const FrameModel& dimension_model = library.model("dimension");
  EXPECT_EQ(value_model.type_name(), symbol("value"));
  EXPECT_EQ(value_model.base_name(), symbol("number"));
  EXPECT_EQ(dimension_model.type_name(), symbol("dimension"));
  EXPECT_EQ(dimension_model.base_name(), symbol("number"));
}

TEST(ParameterTypeRegistrationTest, ComponentSymbolsAreStable) {
  EXPECT_EQ(symbol(Number::component), symbol("number"));
  EXPECT_EQ(symbol(Integer::component), symbol("integer"));
  EXPECT_EQ(symbol(Stringer::component), symbol("string"));
}
