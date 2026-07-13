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
  register_integer_arithmetic_models();
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

std::vector<boost::shared_ptr<const FrameModel> > integer_operands(
    const Library& library, const std::vector<int>& values) {
  std::vector<boost::shared_ptr<const FrameModel> > operands;
  for (size_t i = 0; i < values.size(); ++i) {
    std::unique_ptr<FrameModel> operand = clone_model(library, "const");
    operand->set("value", values[i]);
    operands.push_back(boost::shared_ptr<const FrameModel>(operand.release()));
  }
  return operands;
}

std::vector<std::unique_ptr<Integer> > make_integer_const_models(
    const std::vector<int>& values) {
  std::vector<std::unique_ptr<Integer> > operands;
  for (size_t i = 0; i < values.size(); ++i)
    operands.emplace_back(std::make_unique<IntegerConst>(values[i]));
  return operands;
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
  EXPECT_TRUE(entries.count("sqr"));
  EXPECT_TRUE(entries.count("max"));
  EXPECT_TRUE(entries.count("min"));
  EXPECT_TRUE(entries.count("*"));
  EXPECT_TRUE(entries.count("+"));
  EXPECT_TRUE(entries.count("-"));
  EXPECT_TRUE(entries.count("div"));
  EXPECT_TRUE(entries.count("mod"));
}

TEST(IntegerRegistrationTest, IntegerModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Integer::component);

  EXPECT_TRUE(library.check("const"));
  EXPECT_TRUE(library.check("cond"));
  EXPECT_TRUE(library.check("sqr"));
  EXPECT_TRUE(library.check("max"));
  EXPECT_TRUE(library.check("min"));
  EXPECT_TRUE(library.check("*"));
  EXPECT_TRUE(library.check("+"));
  EXPECT_TRUE(library.check("-"));
  EXPECT_TRUE(library.check("div"));
  EXPECT_TRUE(library.check("mod"));
  EXPECT_TRUE(library.is_derived_from("const", "component"));
  EXPECT_TRUE(library.is_derived_from("cond", "component"));
  EXPECT_TRUE(library.is_derived_from("sqr", "component"));
  EXPECT_TRUE(library.is_derived_from("max", "component"));
  EXPECT_TRUE(library.is_derived_from("min", "component"));
  EXPECT_TRUE(library.is_derived_from("*", "component"));
  EXPECT_TRUE(library.is_derived_from("+", "component"));
  EXPECT_TRUE(library.is_derived_from("-", "component"));
  EXPECT_TRUE(library.is_derived_from("div", "component"));
  EXPECT_TRUE(library.is_derived_from("mod", "component"));
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
  EXPECT_TRUE((std::is_base_of<Integer, IntegerOperand>::value));
  EXPECT_TRUE((std::is_base_of<IntegerOperand, IntegerSqr>::value));
  EXPECT_TRUE((std::is_base_of<Integer, IntegerOperands>::value));
  EXPECT_TRUE((std::is_base_of<IntegerOperands, IntegerMax>::value));
  EXPECT_TRUE((std::is_base_of<IntegerOperands, IntegerMin>::value));
  EXPECT_TRUE((std::is_base_of<IntegerOperands, IntegerProduct>::value));
  EXPECT_TRUE((std::is_base_of<IntegerOperands, IntegerSum>::value));
  EXPECT_TRUE((std::is_base_of<IntegerOperands, IntegerSubtract>::value));
  EXPECT_TRUE((std::is_base_of<IntegerOperands, IntegerDivide>::value));
  EXPECT_TRUE((std::is_base_of<IntegerDivide, IntegerModulo>::value));
  EXPECT_TRUE((std::is_constructible<IntegerConst, int>::value));
  EXPECT_TRUE((std::is_constructible<IntegerCond::Clause, std::unique_ptr<Boolean>, int>::value));
  EXPECT_TRUE((std::is_constructible<IntegerCond, std::vector<IntegerCond::Clause>>::value));
  EXPECT_TRUE((std::is_abstract<IntegerOperand>::value));
  EXPECT_TRUE((std::is_abstract<IntegerOperands>::value));
  EXPECT_TRUE((std::is_constructible<IntegerSqr, std::unique_ptr<Integer>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerMax, std::vector<std::unique_ptr<Integer>>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerMin, std::vector<std::unique_ptr<Integer>>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerProduct, std::vector<std::unique_ptr<Integer>>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerSum, std::vector<std::unique_ptr<Integer>>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerSubtract, std::vector<std::unique_ptr<Integer>>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerDivide, std::vector<std::unique_ptr<Integer>>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerModulo, std::vector<std::unique_ptr<Integer>>>::value));
  EXPECT_TRUE((std::is_constructible<IntegerConst, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerCond, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerSqr, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerMax, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerMin, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerProduct, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerSum, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerSubtract, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerDivide, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<IntegerModulo, const BlockModel&>::value));
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

TEST(IntegerExposureTest, IntegerArithmeticClassesHaveDirectConstructors) {
  register_test_models();
  Metalib metalib(load_test_frame);

  IntegerSqr sqr(std::make_unique<IntegerConst>(4));
  IntegerMax max(make_integer_const_models({1, 7, 3}));
  IntegerMin min(make_integer_const_models({1, 7, 3}));
  IntegerProduct product(make_integer_const_models({2, 3, 4}));
  IntegerSum sum(make_integer_const_models({2, 3, 4}));
  IntegerSubtract subtract(make_integer_const_models({10, 3, 2}));
  IntegerDivide divide(make_integer_const_models({12, 3}));
  IntegerModulo mod(make_integer_const_models({14, 4}));

  EXPECT_TRUE(sqr.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(sqr.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(sqr.value(Scope::null()), 16);
  EXPECT_EQ(max.value(Scope::null()), 7);
  EXPECT_EQ(min.value(Scope::null()), 1);
  EXPECT_EQ(product.value(Scope::null()), 24);
  EXPECT_EQ(sum.value(Scope::null()), 9);
  EXPECT_EQ(subtract.value(Scope::null()), 5);
  EXPECT_TRUE(divide.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(divide.value(Scope::null()), 4);
  EXPECT_TRUE(mod.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(mod.value(Scope::null()), 2);
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

TEST(IntegerExposureTest, IntegerArithmeticClassesCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& integer_library = metalib.library(Integer::component);

  BlockTop context(metalib, Treelog::null(), metalib);

  std::unique_ptr<FrameModel> sqr_frame = clone_model(integer_library, "sqr");
  std::unique_ptr<FrameModel> sqr_operand = clone_model(integer_library, "const");
  sqr_operand->set("value", 4);
  sqr_frame->set("operand", *sqr_operand);
  BlockModel sqr_block(context, *sqr_frame, "sqr");
  IntegerSqr sqr(sqr_block);
  EXPECT_TRUE(sqr.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(sqr.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(sqr.value(Scope::null()), 16);

  std::unique_ptr<FrameModel> max_frame = clone_model(integer_library, "max");
  max_frame->set("operands", integer_operands(integer_library, {1, 7, 3}));
  BlockModel max_block(context, *max_frame, "max");
  IntegerMax max(max_block);
  EXPECT_TRUE(max.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(max.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(max.value(Scope::null()), 7);

  std::unique_ptr<FrameModel> subtract_frame = clone_model(integer_library, "-");
  subtract_frame->set("operands", integer_operands(integer_library, {10, 3, 2}));
  BlockModel subtract_block(context, *subtract_frame, "-");
  IntegerSubtract subtract(subtract_block);
  EXPECT_TRUE(subtract.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(subtract.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(subtract.value(Scope::null()), 5);

  std::unique_ptr<FrameModel> divide_frame = clone_model(integer_library, "div");
  divide_frame->set("operands", integer_operands(integer_library, {12, 3}));
  BlockModel divide_block(context, *divide_frame, "div");
  IntegerDivide divide(divide_block);
  EXPECT_TRUE(divide.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(divide.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(divide.value(Scope::null()), 4);

  std::unique_ptr<FrameModel> mod_frame = clone_model(integer_library, "mod");
  mod_frame->set("operands", integer_operands(integer_library, {14, 4}));
  BlockModel mod_block(context, *mod_frame, "mod");
  IntegerModulo mod(mod_block);
  EXPECT_TRUE(mod.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(mod.check(Scope::null(), Treelog::null()));
  EXPECT_EQ(mod.value(Scope::null()), 2);
}
