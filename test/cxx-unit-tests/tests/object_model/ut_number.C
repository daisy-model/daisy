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
#include "object_model/parameter_types/number.h"
#include "object_model/unit.h"
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
  register_number_models();
  register_number_const_models();
}

}  // namespace

TEST(NumberRegistrationTest, NumberLibraryContainsExpectedModels) {
  register_test_models();
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Number::component));
  const Library& library = metalib.library(Number::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("const"));
}

TEST(NumberRegistrationTest, NumberConstModelHasExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);

  EXPECT_TRUE(library.check("const"));
  EXPECT_TRUE(library.is_derived_from("const", "component"));
  EXPECT_EQ(library.base_model("const"), symbol("component"));

  const FrameModel& const_model = library.model("const");
  EXPECT_EQ(const_model.type_name(), symbol("const"));
  EXPECT_EQ(const_model.base_name(), symbol("component"));
}

TEST(NumberRegistrationTest, NumberComponentMetadataIsStable) {
  EXPECT_EQ(symbol(Number::component), symbol("number"));
}

TEST(NumberExposureTest, NumberConstIsPublicAndDirectlyConstructible) {
  EXPECT_TRUE((std::is_base_of<Number, NumberConst>::value));
  EXPECT_TRUE((std::is_constructible<NumberConst, double, const Unit&>::value));
  EXPECT_TRUE((std::is_constructible<NumberConst, const BlockModel&>::value));
}

TEST(NumberExposureTest, NumberConstHasDirectValueConstructor) {
  register_test_models();
  Metalib metalib(load_test_frame);
  NumberConst number(17.5, metalib.units().get_unit(Units::cm()));

  EXPECT_FALSE(number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(number.value(Scope::null()), 17.5);
  EXPECT_EQ(number.dimension(Scope::null()), Units::cm());
}

TEST(NumberExposureTest, NumberConstCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "const");
  frame->set("value", 17.5, Units::cm());

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "const");
  NumberConst number(block);

  EXPECT_FALSE(number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(number.value(Scope::null()), 17.5);
  EXPECT_EQ(number.dimension(Scope::null()), Units::cm());
}
