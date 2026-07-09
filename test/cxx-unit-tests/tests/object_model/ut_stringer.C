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

std::unique_ptr<FrameModel> clone_model(const Library& library,
                                        const symbol name) {
  return std::unique_ptr<FrameModel>(&library.model(name).clone());
}

void register_test_models() {
  register_unit_models();
  register_number_models();
  register_stringer_models();
}

}  // namespace

TEST(StringerRegistrationTest, StringerLibraryContainsExpectedModels) {
  register_test_models();
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

TEST(StringerRegistrationTest, StringerModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Stringer::component);

  EXPECT_TRUE(library.is_derived_from("value", "number"));
  EXPECT_TRUE(library.is_derived_from("dimension", "number"));
  EXPECT_TRUE(library.is_derived_from("identity", "component"));
  EXPECT_EQ(library.base_model("value"), symbol("component"));
  EXPECT_EQ(library.base_model("identity"), symbol("component"));

  const FrameModel& number_model = library.model("number");
  EXPECT_EQ(number_model.type_name(), symbol("number"));
  EXPECT_EQ(number_model.base_name(), symbol("component"));
}

TEST(StringerRegistrationTest, StringerComponentMetadataIsStable) {
  EXPECT_EQ(symbol(Stringer::component), symbol("string"));
}

TEST(StringerExposureTest, StringerClassesArePublicTypes) {
  EXPECT_TRUE((std::is_base_of<Stringer, StringerNumber>::value));
  EXPECT_TRUE((std::is_base_of<StringerNumber, StringerValue>::value));
  EXPECT_TRUE((std::is_base_of<StringerNumber, StringerDimension>::value));
  EXPECT_TRUE((std::is_base_of<Stringer, StringerIdentity>::value));
  EXPECT_TRUE((std::is_abstract<StringerNumber>::value));
  EXPECT_TRUE((std::is_constructible<StringerValue, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<StringerDimension, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<StringerIdentity, symbol>::value));
  EXPECT_TRUE((std::is_constructible<StringerIdentity, const BlockModel&>::value));
}

TEST(StringerExposureTest, StringerIdentityHasDirectValueConstructor) {
  StringerIdentity stringer("hello");

  EXPECT_FALSE(stringer.missing(Scope::null()));
  EXPECT_EQ(stringer.value(Scope::null()), symbol("hello"));
}

TEST(StringerExposureTest, StringerIdentityCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Stringer::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "identity");
  frame->set("value", "hello");

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "identity");
  StringerIdentity stringer(block);

  EXPECT_FALSE(stringer.missing(Scope::null()));
  EXPECT_EQ(stringer.value(Scope::null()), symbol("hello"));
}
