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

std::unique_ptr<FrameModel> clone_model(const Library& library,
                                        const symbol name) {
  return std::unique_ptr<FrameModel>(&library.model(name).clone());
}

std::vector<boost::shared_ptr<const FrameModel> > clone_operands(
    const Library& library, const std::vector<symbol>& names) {
  std::vector<boost::shared_ptr<const FrameModel> > operands;
  for (size_t i = 0; i < names.size(); ++i)
    operands.push_back(
        boost::shared_ptr<const FrameModel>(&library.model(names[i]).clone()));
  return operands;
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

TEST(BooleanExposureTest, BooleanLeafClassesArePublicTypes) {
  EXPECT_TRUE((std::is_base_of<Boolean, BooleanTrue>::value));
  EXPECT_TRUE((std::is_base_of<Boolean, BooleanFalse>::value));
  EXPECT_TRUE((std::is_constructible<BooleanTrue, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<BooleanFalse, const BlockModel&>::value));
}

TEST(BooleanExposureTest, BooleanOperandClassesArePublicTypes) {
  EXPECT_TRUE((std::is_base_of<Boolean, BooleanOperands>::value));
  EXPECT_TRUE((std::is_base_of<BooleanOperands, BooleanAnd>::value));
  EXPECT_TRUE((std::is_base_of<BooleanOperands, BooleanOr>::value));
  EXPECT_TRUE((std::is_base_of<BooleanOperands, BooleanXOr>::value));
  EXPECT_TRUE((std::is_base_of<BooleanOperands, BooleanNot>::value));
  EXPECT_TRUE((std::is_abstract<BooleanOperands>::value));
  EXPECT_TRUE((std::is_constructible<BooleanAnd, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<BooleanOr, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<BooleanXOr, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<BooleanNot, const BlockModel&>::value));
}

TEST(BooleanExposureTest, BooleanLeafClassesCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Boolean::component);
  BlockTop context(metalib, Treelog::null(), metalib);

  std::unique_ptr<FrameModel> true_frame = clone_model(library, "true");
  BlockModel true_block(context, *true_frame, "true");
  BooleanTrue true_model(true_block);

  std::unique_ptr<FrameModel> false_frame = clone_model(library, "false");
  BlockModel false_block(context, *false_frame, "false");
  BooleanFalse false_model(false_block);

  EXPECT_TRUE(true_model.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(true_model.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(true_model.missing(Scope::null()));
  EXPECT_TRUE(true_model.value(Scope::null()));

  EXPECT_TRUE(false_model.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(false_model.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(false_model.missing(Scope::null()));
  EXPECT_FALSE(false_model.value(Scope::null()));
}

TEST(BooleanExposureTest, BooleanCompositeClassesCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Boolean::component);
  BlockTop context(metalib, Treelog::null(), metalib);

  std::unique_ptr<FrameModel> and_frame = clone_model(library, "and");
  and_frame->set("operands", clone_operands(library, {"true", "false"}));
  BlockModel and_block(context, *and_frame, "and");
  BooleanAnd and_model(and_block);
  EXPECT_TRUE(and_model.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(and_model.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(and_model.missing(Scope::null()));
  EXPECT_FALSE(and_model.value(Scope::null()));

  std::unique_ptr<FrameModel> or_frame = clone_model(library, "or");
  or_frame->set("operands", clone_operands(library, {"true", "false"}));
  BlockModel or_block(context, *or_frame, "or");
  BooleanOr or_model(or_block);
  EXPECT_TRUE(or_model.value(Scope::null()));

  std::unique_ptr<FrameModel> xor_frame = clone_model(library, "xor");
  xor_frame->set("operands", clone_operands(library, {"true", "false"}));
  BlockModel xor_block(context, *xor_frame, "xor");
  BooleanXOr xor_model(xor_block);
  EXPECT_TRUE(xor_model.value(Scope::null()));

  std::unique_ptr<FrameModel> not_frame = clone_model(library, "not");
  not_frame->set("operands", clone_operands(library, {"false"}));
  BlockModel not_block(context, *not_frame, "not");
  BooleanNot not_model(not_block);
  EXPECT_TRUE(not_model.value(Scope::null()));
}
