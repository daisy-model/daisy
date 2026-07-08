#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/frame_model.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/parameter_types/boolean.h"
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

void register_test_models() {
  register_unit_models();
  register_boolean_models();
}

}  // namespace

TEST(BooleanRegistrationTest, BooleanLibraryContainsExpectedModels) {
  register_test_models();
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Boolean::component));
  const Library& library = metalib.library(Boolean::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("true"));
  EXPECT_TRUE(entries.count("false"));
  EXPECT_TRUE(entries.count("operands"));
  EXPECT_TRUE(entries.count("and"));
  EXPECT_TRUE(entries.count("or"));
  EXPECT_TRUE(entries.count("xor"));
  EXPECT_TRUE(entries.count("not"));
}

TEST(BooleanRegistrationTest, BooleanCompositeModelsDeriveFromOperandsBase) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Boolean::component);

  EXPECT_TRUE(library.is_derived_from("true", "component"));
  EXPECT_TRUE(library.is_derived_from("false", "component"));
  EXPECT_TRUE(library.is_derived_from("operands", "component"));
  EXPECT_TRUE(library.is_derived_from("and", "operands"));
  EXPECT_TRUE(library.is_derived_from("or", "operands"));
  EXPECT_FALSE(library.is_derived_from("xor", "operands"));
  EXPECT_FALSE(library.is_derived_from("not", "operands"));
  EXPECT_TRUE(library.is_derived_from("xor", "component"));
  EXPECT_TRUE(library.is_derived_from("not", "component"));

  EXPECT_EQ(library.base_model("and"), symbol("component"));
  EXPECT_EQ(library.base_model("or"), symbol("component"));
  EXPECT_EQ(library.base_model("xor"), symbol("component"));
  EXPECT_EQ(library.base_model("not"), symbol("component"));
}

TEST(BooleanRegistrationTest, BooleanLeafModelsHaveNoBaseModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Boolean::component);

  const FrameModel& true_model = library.model("true");
  const FrameModel& false_model = library.model("false");
  const FrameModel& operands_model = library.model("operands");

  EXPECT_EQ(true_model.base_name(), symbol("component"));
  EXPECT_EQ(false_model.base_name(), symbol("component"));
  EXPECT_EQ(operands_model.base_name(), symbol("component"));
  EXPECT_EQ(true_model.type_name(), symbol("true"));
  EXPECT_EQ(false_model.type_name(), symbol("false"));
}

TEST(BooleanRegistrationTest, BooleanComponentMetadataIsStable) {
  EXPECT_EQ(symbol(Boolean::component), symbol("boolean"));
}
