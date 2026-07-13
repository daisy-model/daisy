#include <memory>
#include <set>
#include <boost/shared_ptr.hpp>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/block_model.h"
#include "object_model/block_top.h"
#include "object_model/frame_model.h"
#include "object_model/frame_submodel.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/parameter_types/boolean.h"
#include "object_model/parameter_types/integer.h"
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
  register_boolean_models();
  register_integer_models();
}

void load_integer_cond_clause_frame(Frame& frame) {
  frame.declare_object("condition", Boolean::component, "Condition to test for.");
  frame.declare_integer("value", Attribute::Const, "Value to return.");
  frame.order("condition", "value");
}

boost::shared_ptr<const FrameSubmodel> make_integer_cond_clause(const Library& boolean_library,
                                                                symbol boolean_name,
                                                                int value) {
  boost::shared_ptr<FrameSubmodel> clause(new FrameSubmodel(load_integer_cond_clause_frame));
  std::unique_ptr<FrameModel> condition = clone_model(boolean_library, boolean_name);
  clause->set("condition", *condition);
  clause->set("value", value);
  return clause;
}

}  // namespace

TEST(IntegerRegistrationTest, IntegerLibraryContainsExpectedModels) {
  register_test_models();
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Integer::component));
  const Library& library = metalib.library(Integer::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("const"));
  EXPECT_TRUE(entries.count("cond"));
}

TEST(IntegerRegistrationTest, IntegerConstModelHasExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Integer::component);

  EXPECT_TRUE(library.check("const"));
  EXPECT_TRUE(library.check("cond"));
  EXPECT_TRUE(library.is_derived_from("const", "component"));
  EXPECT_TRUE(library.is_derived_from("cond", "component"));
  EXPECT_EQ(library.base_model("const"), symbol("component"));

  const FrameModel& const_model = library.model("const");
  EXPECT_EQ(const_model.type_name(), symbol("const"));
  EXPECT_EQ(const_model.base_name(), symbol("component"));
}

TEST(IntegerRegistrationTest, IntegerComponentMetadataIsStable) {
  EXPECT_EQ(symbol(Integer::component), symbol("integer"));
}

TEST(IntegerExposureTest, IntegerConstIsPublicAndDirectlyConstructible) {
  EXPECT_TRUE((std::is_base_of<Integer, IntegerConst>::value));
  EXPECT_TRUE((std::is_base_of<Integer, IntegerCond>::value));
  EXPECT_TRUE((std::is_constructible<IntegerConst, int>::value));
  EXPECT_TRUE((std::is_constructible<IntegerCond::Clause, std::unique_ptr<Boolean>, int>::value));
  EXPECT_TRUE((std::is_constructible<IntegerCond, std::vector<IntegerCond::Clause>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerConst, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerCond, const BlockModel&>::value));
}

TEST(IntegerExposureTest, IntegerConstHasDirectValueConstructor) {
  IntegerConst integer(17);

  EXPECT_FALSE(integer.missing(Scope::null()));
  EXPECT_EQ(integer.value(Scope::null()), 17);
}

TEST(IntegerExposureTest, IntegerConstCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Integer::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "const");
  frame->set("value", 17);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "const");
  IntegerConst integer(block);

  EXPECT_FALSE(integer.missing(Scope::null()));
  EXPECT_EQ(integer.value(Scope::null()), 17);
}

TEST(IntegerExposureTest, IntegerCondHasDirectClauseConstructor) {
  register_test_models();
  Metalib metalib(load_test_frame);

  std::vector<IntegerCond::Clause> clauses;
  clauses.emplace_back(std::make_unique<BooleanFalse>(), 10);
  clauses.emplace_back(std::make_unique<BooleanTrue>(), 17);
  IntegerCond integer(std::move(clauses));

  EXPECT_TRUE(integer.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(integer.check(Scope::null(), Treelog::null()));
  EXPECT_FALSE(integer.missing(Scope::null()));
  EXPECT_EQ(integer.value(Scope::null()), 17);
}

TEST(IntegerExposureTest, IntegerCondCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& integer_library = metalib.library(Integer::component);
  const Library& boolean_library = metalib.library(Boolean::component);
  std::unique_ptr<FrameModel> frame = clone_model(integer_library, "cond");
  std::vector<boost::shared_ptr<const FrameSubmodel>> clauses;
  clauses.push_back(make_integer_cond_clause(boolean_library, "false", 10));
  clauses.push_back(make_integer_cond_clause(boolean_library, "true", 17));
  frame->set("clauses", clauses);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "cond");
  IntegerCond integer(block);

  EXPECT_TRUE(integer.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(integer.check(Scope::null(), Treelog::null()));
  EXPECT_FALSE(integer.missing(Scope::null()));
  EXPECT_EQ(integer.value(Scope::null()), 17);
}
