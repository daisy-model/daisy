#include <memory>
#include <set>
#include <boost/shared_ptr.hpp>
#include <cmath>
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
#include "object_model/parameter_types/number.h"
#include "object_model/plf.h"
#include "object_model/unit.h"
#include "object_model/units.h"
#include "util/scope.h"

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
  register_function_models();
  register_number_models();
  register_number_apply_models();
  register_number_arithmetic_models();
  register_number_const_models();
  register_number_lisp_models();
  register_number_plf_models();
}

class NumberScope : public Scope {
public:
  void entries(std::set<symbol>& result) const override {
    result.insert("x");
    result.insert("water");
  }

  Attribute::type lookup(symbol name) const override {
    return (name == symbol("x") || name == symbol("water"))
      ? Attribute::Number
      : Attribute::Error;
  }

  symbol dimension(symbol name) const override {
    if (name == symbol("x")) {
      return "kg";
    }
    if (name == symbol("water")) {
      return Units::cm();
    }
    return Attribute::Unknown();
  }

  symbol description(symbol) const override { return "test"; }

  bool check(symbol name) const override {
    return name == symbol("x") || name == symbol("water");
  }

  double number(symbol name) const override {
    if (name == symbol("x")) {
      return 42.0;
    }
    if (name == symbol("water")) {
      return 175.0;
    }
    return 0.0;
  }
};

void load_number_plf_point_frame(Frame& frame) {
  frame.declare("x", Attribute::User(), Attribute::Const, "Operand.");
  frame.declare("y", Attribute::User(), Attribute::Const, "Value.");
  frame.order("x", "y");
}

boost::shared_ptr<const FrameSubmodel> make_number_plf_point(double x,
                                                             symbol x_dimension,
                                                             double y,
                                                             symbol y_dimension) {
  boost::shared_ptr<FrameSubmodel> point(new FrameSubmodel(load_number_plf_point_frame));
  point->set("x", x, x_dimension);
  point->set("y", y, y_dimension);
  return point;
}

std::vector<std::unique_ptr<Number>> make_const_operands(
    const Metalib& metalib,
    const std::vector<double>& values,
    symbol dimension = Units::cm()) {
  std::vector<std::unique_ptr<Number>> operands;
  operands.reserve(values.size());
  for (size_t i = 0; i < values.size(); ++i) {
    operands.push_back(
        std::make_unique<NumberConst>(values[i], metalib.units().get_unit(dimension)));
  }
  return operands;
}

boost::shared_ptr<const FrameModel> make_number_const_frame(const Library& library,
                                                            double value,
                                                            symbol dimension) {
  std::unique_ptr<FrameModel> frame = clone_model(library, "const");
  frame->set("value", value, dimension);
  return boost::shared_ptr<const FrameModel>(frame.release());
}

void load_number_let_clause_frame(Frame& frame) {
  frame.declare_string("identifier", Attribute::Const, "Identifier to bind.");
  frame.declare_object("expr", Number::component, " Value to give it.");
  frame.order("identifier", "expr");
}

boost::shared_ptr<const FrameSubmodel> make_number_let_clause(
    const Library& number_library,
    symbol identifier,
    double value,
    symbol dimension) {
  boost::shared_ptr<FrameSubmodel> clause(new FrameSubmodel(load_number_let_clause_frame));
  std::unique_ptr<FrameModel> expr = clone_model(number_library, "const");
  expr->set("value", value, dimension);
  clause->set("identifier", identifier);
  clause->set("expr", *expr);
  return clause;
}

}  // namespace

TEST(NumberRegistrationTest, NumberLibraryContainsExpectedModels) {
  register_test_models();
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
  EXPECT_TRUE(entries.count("log10"));
  EXPECT_TRUE(entries.count("ln"));
  EXPECT_TRUE(entries.count("exp"));
  EXPECT_TRUE(entries.count("sqrt"));
  EXPECT_TRUE(entries.count("sqr"));
  EXPECT_TRUE(entries.count("pow"));
  EXPECT_TRUE(entries.count("apply"));
  EXPECT_TRUE(entries.count("let"));
  EXPECT_TRUE(entries.count("if"));
  EXPECT_TRUE(entries.count("max"));
  EXPECT_TRUE(entries.count("min"));
  EXPECT_TRUE(entries.count("*"));
  EXPECT_TRUE(entries.count("+"));
  EXPECT_TRUE(entries.count("-"));
  EXPECT_TRUE(entries.count("/"));
  EXPECT_TRUE(entries.count("plf"));
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

TEST(NumberRegistrationTest, NumberPlfModelHasExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);

  EXPECT_TRUE(library.check("plf"));
  EXPECT_TRUE(library.is_derived_from("plf", "component"));
  EXPECT_EQ(library.base_model("plf"), symbol("component"));

  const FrameModel& plf_model = library.model("plf");
  EXPECT_EQ(plf_model.type_name(), symbol("plf"));
  EXPECT_EQ(plf_model.base_name(), symbol("component"));
}

TEST(NumberRegistrationTest, NumberUnaryArithmeticModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);

  const std::vector<symbol> names = {"log10", "ln", "exp", "sqrt", "sqr"};
  for (size_t i = 0; i < names.size(); ++i) {
    EXPECT_TRUE(library.check(names[i]));
    EXPECT_TRUE(library.is_derived_from(names[i], "component"));
    EXPECT_EQ(library.base_model(names[i]), symbol("component"));

    const FrameModel& model = library.model(names[i]);
    EXPECT_EQ(model.type_name(), names[i]);
    EXPECT_EQ(model.base_name(), symbol("component"));
  }
}

TEST(NumberRegistrationTest, NumberPowAndSharedOperandModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);

  const std::vector<symbol> names = {"pow", "apply", "let", "if", "max", "min", "*", "+", "-", "/"};
  for (size_t i = 0; i < names.size(); ++i) {
    EXPECT_TRUE(library.check(names[i]));
    EXPECT_TRUE(library.is_derived_from(names[i], "component"));
    EXPECT_EQ(library.base_model(names[i]), symbol("component"));

    const FrameModel& model = library.model(names[i]);
    EXPECT_EQ(model.type_name(), names[i]);
    EXPECT_EQ(model.base_name(), symbol("component"));
  }
}

TEST(NumberRegistrationTest, NumberComponentMetadataIsStable) {
  EXPECT_EQ(symbol(Number::component), symbol("number"));
}

TEST(NumberExposureTest, NumberConstIsPublicAndDirectlyConstructible) {
  EXPECT_TRUE((std::is_base_of<Number, NumberConst>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberX>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberGet>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberFetchGet>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberFetch>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberApply>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberLet>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberIf>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberChild>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberOperand>::value));
  EXPECT_TRUE((std::is_base_of<NumberChild, NumberIdentity>::value));
  EXPECT_TRUE((std::is_base_of<NumberChild, NumberConvert>::value));
  EXPECT_TRUE((std::is_base_of<NumberChild, NumberDim>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperand, NumberLog10>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperand, NumberLn>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperand, NumberExp>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperand, NumberSqrt>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperand, NumberSqr>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberPow>::value));
  EXPECT_TRUE((std::is_base_of<Number, NumberOperands>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperands, NumberMax>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperands, NumberMin>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperands, NumberProduct>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperands, NumberSum>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperands, NumberSubtract>::value));
  EXPECT_TRUE((std::is_base_of<NumberOperands, NumberDivide>::value));
  EXPECT_TRUE((std::is_abstract<NumberChild>::value));
  EXPECT_TRUE((std::is_abstract<NumberOperand>::value));
  EXPECT_TRUE((std::is_abstract<NumberOperands>::value));
  EXPECT_TRUE((std::is_constructible<NumberConst, double, const Unit&>::value));
  EXPECT_TRUE((std::is_constructible<NumberX>::value));
  EXPECT_TRUE((std::is_constructible<NumberGet, symbol, const Unit&>::value));
  EXPECT_TRUE((std::is_constructible<NumberFetchGet, symbol>::value));
  EXPECT_TRUE((std::is_constructible<NumberFetchGet, const BlockModel&, symbol>::value));
  EXPECT_TRUE((std::is_constructible<NumberFetch, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberApply, std::unique_ptr<Function>, double, symbol>::value));
  EXPECT_TRUE((std::is_constructible<NumberLet::Clause, symbol, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberLet, std::vector<NumberLet::Clause>, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberIf, std::unique_ptr<Boolean>, std::unique_ptr<Number>, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberIdentity, std::unique_ptr<Number>, const Units&>::value));
  EXPECT_TRUE((std::is_constructible<NumberIdentity, std::unique_ptr<Number>, const Units&, symbol>::value));
  EXPECT_TRUE((std::is_constructible<NumberConvert, std::unique_ptr<Number>, const Units&, symbol>::value));
  EXPECT_TRUE((std::is_constructible<NumberDim, std::unique_ptr<Number>, symbol, bool>::value));
  EXPECT_TRUE((std::is_constructible<NumberLog10, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberLn, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberExp, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberSqrt, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberSqr, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberPow, std::unique_ptr<Number>, std::unique_ptr<Number>>::value));
  EXPECT_TRUE((std::is_constructible<NumberMax, std::vector<std::unique_ptr<Number>>>::value));
  EXPECT_TRUE((std::is_constructible<NumberMin, std::vector<std::unique_ptr<Number>>>::value));
  EXPECT_TRUE((std::is_constructible<NumberProduct, std::vector<std::unique_ptr<Number>>>::value));
  EXPECT_TRUE((std::is_constructible<NumberSum, std::vector<std::unique_ptr<Number>>>::value));
  EXPECT_TRUE((std::is_constructible<NumberSubtract, std::vector<std::unique_ptr<Number>>>::value));
  EXPECT_TRUE((std::is_constructible<NumberDivide, std::vector<std::unique_ptr<Number>>>::value));
  EXPECT_TRUE((std::is_constructible<NumberPLF, std::unique_ptr<Number>, symbol, symbol, const PLF&>::value));
  EXPECT_TRUE((std::is_constructible<NumberConst, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberX, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberGet, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberFetch, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberApply, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberLet, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberIf, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberIdentity, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberConvert, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberDim, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberLog10, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberLn, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberExp, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberSqrt, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberSqr, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberPow, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberMax, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberMin, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberProduct, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberSum, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberSubtract, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberDivide, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<NumberPLF, const BlockModel&>::value));
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

TEST(NumberExposureTest, NumberXUsesCurrentScopeValue) {
  register_test_models();
  Metalib metalib(load_test_frame);
  NumberScope scope;
  NumberX number;

  EXPECT_TRUE(number.initialize(metalib.units(), scope, Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), scope, Treelog::null()));
  EXPECT_FALSE(number.missing(scope));
  EXPECT_DOUBLE_EQ(number.value(scope), 42.0);
  EXPECT_EQ(number.dimension(scope), symbol("kg"));
}

TEST(NumberExposureTest, NumberGetHasDirectConstructor) {
  register_test_models();
  Metalib metalib(load_test_frame);
  NumberScope scope;
  NumberGet number("water", metalib.units().get_unit(Units::mm()));

  EXPECT_TRUE(number.initialize(metalib.units(), scope, Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), scope, Treelog::null()));
  EXPECT_FALSE(number.missing(scope));
  EXPECT_DOUBLE_EQ(number.value(scope), 1750.0);
  EXPECT_EQ(number.dimension(scope), Units::mm());
}

TEST(NumberExposureTest, NumberGetCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  NumberScope scope;
  const Library& library = metalib.library(Number::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "get");
  frame->set("name", "water");
  frame->set("dimension", Units::mm());

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "get");
  NumberGet number(block);

  EXPECT_TRUE(number.initialize(metalib.units(), scope, Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), scope, Treelog::null()));
  EXPECT_DOUBLE_EQ(number.value(scope), 1750.0);
  EXPECT_EQ(number.dimension(scope), Units::mm());
}

TEST(NumberExposureTest, NumberFetchGetHasDirectConstructor) {
  register_test_models();
  Metalib metalib(load_test_frame);
  NumberScope scope;
  NumberFetchGet number("water");

  EXPECT_TRUE(number.initialize(metalib.units(), scope, Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), scope, Treelog::null()));
  EXPECT_FALSE(number.missing(scope));
  EXPECT_DOUBLE_EQ(number.value(scope), 175.0);
  EXPECT_EQ(number.dimension(scope), Units::cm());
}

TEST(NumberExposureTest, NumberFetchHasDirectChildConstructor) {
  register_test_models();
  Metalib metalib(load_test_frame);
  NumberScope scope;
  NumberFetch number(std::make_unique<NumberFetchGet>("water"));

  EXPECT_TRUE(number.initialize(metalib.units(), scope, Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), scope, Treelog::null()));
  EXPECT_FALSE(number.missing(scope));
  EXPECT_DOUBLE_EQ(number.value(scope), 175.0);
  EXPECT_EQ(number.dimension(scope), Units::cm());
}

TEST(NumberExposureTest, NumberFetchCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  NumberScope scope;
  const Library& library = metalib.library(Number::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "fetch");
  frame->set("name", "water");

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "fetch");
  NumberFetch number(block);

  EXPECT_TRUE(number.initialize(metalib.units(), scope, Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), scope, Treelog::null()));
  EXPECT_FALSE(number.missing(scope));
  EXPECT_DOUBLE_EQ(number.value(scope), 175.0);
  EXPECT_EQ(number.dimension(scope), Units::cm());
}

TEST(NumberExposureTest, NumberApplyHasDirectConstructor) {
  NumberApply number(std::make_unique<FunctionConst>(8.5), 3.0, Units::mm());

  EXPECT_FALSE(number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(number.value(Scope::null()), 8.5);
  EXPECT_EQ(number.dimension(Scope::null()), Units::mm());
}

TEST(NumberExposureTest, NumberApplyCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& number_library = metalib.library(Number::component);
  const Library& function_library = metalib.library(Function::component);
  std::unique_ptr<FrameModel> frame = clone_model(number_library, "apply");
  std::unique_ptr<FrameModel> function = clone_model(function_library, "const");
  function->set("domain", Units::cm());
  function->set("range", Units::mm());
  function->set("value", 8.5);
  frame->set("function", *function);
  frame->set("operand", 3.0, Units::cm());

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "apply");
  NumberApply number(block);

  EXPECT_TRUE(number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(number.value(Scope::null()), 8.5);
  EXPECT_EQ(number.dimension(Scope::null()), Units::mm());
}

TEST(NumberExposureTest, NumberLispClassesHaveDirectConstructors) {
  register_test_models();
  Metalib metalib(load_test_frame);

  std::vector<NumberLet::Clause> clauses;
  clauses.emplace_back("bound",
                       std::make_unique<NumberConst>(17.5, metalib.units().get_unit(Units::cm())));
  NumberLet let_number(std::move(clauses), std::make_unique<NumberFetchGet>("bound"));
  EXPECT_TRUE(let_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(let_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(let_number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(let_number.value(Scope::null()), 17.5);
  EXPECT_EQ(let_number.dimension(Scope::null()), Units::cm());

  NumberIf if_number(std::make_unique<BooleanTrue>(),
                     std::make_unique<NumberConst>(17.5, metalib.units().get_unit(Units::cm())),
                     std::make_unique<NumberConst>(11.0, metalib.units().get_unit(Units::cm())));
  EXPECT_TRUE(if_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(if_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(if_number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(if_number.value(Scope::null()), 17.5);
  EXPECT_EQ(if_number.dimension(Scope::null()), Units::cm());
}

TEST(NumberExposureTest, NumberLispClassesCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& number_library = metalib.library(Number::component);
  const Library& boolean_library = metalib.library(Boolean::component);

  BlockTop context(metalib, Treelog::null(), metalib);

  std::unique_ptr<FrameModel> let_frame = clone_model(number_library, "let");
  std::vector<boost::shared_ptr<const FrameSubmodel>> clauses;
  clauses.push_back(make_number_let_clause(number_library, "bound", 17.5, Units::cm()));
  let_frame->set("clauses", clauses);
  std::unique_ptr<FrameModel> let_expr = clone_model(number_library, "fetch");
  let_expr->set("name", "bound");
  let_frame->set("expr", *let_expr);
  BlockModel let_block(context, *let_frame, "let");
  NumberLet let_number(let_block);
  EXPECT_TRUE(let_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(let_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(let_number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(let_number.value(Scope::null()), 17.5);
  EXPECT_EQ(let_number.dimension(Scope::null()), Units::cm());

  std::unique_ptr<FrameModel> if_frame = clone_model(number_library, "if");
  std::unique_ptr<FrameModel> condition = clone_model(boolean_library, "true");
  std::unique_ptr<FrameModel> then_frame = clone_model(number_library, "const");
  then_frame->set("value", 17.5, Units::cm());
  std::unique_ptr<FrameModel> else_frame = clone_model(number_library, "const");
  else_frame->set("value", 11.0, Units::cm());
  if_frame->set("if", *condition);
  if_frame->set("then", *then_frame);
  if_frame->set("else", *else_frame);
  BlockModel if_block(context, *if_frame, "if");
  NumberIf if_number(if_block);
  EXPECT_TRUE(if_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(if_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(if_number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(if_number.value(Scope::null()), 17.5);
  EXPECT_EQ(if_number.dimension(Scope::null()), Units::cm());
}

TEST(NumberExposureTest, NumberIdentityAndDerivedClassesHaveDirectChildConstructors) {
  register_test_models();
  Metalib metalib(load_test_frame);

  NumberIdentity identity(
      std::make_unique<NumberConst>(175.0, metalib.units().get_unit(Units::cm())),
      metalib.units());
  EXPECT_TRUE(identity.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(identity.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(identity.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(identity.value(Scope::null()), 175.0);
  EXPECT_EQ(identity.dimension(Scope::null()), Units::cm());

  NumberIdentity identity_in_m(
      std::make_unique<NumberConst>(175.0, metalib.units().get_unit(Units::cm())),
      metalib.units(), Units::mm());
  EXPECT_DOUBLE_EQ(identity_in_m.value(Scope::null()), 1750.0);
  EXPECT_EQ(identity_in_m.dimension(Scope::null()), Units::mm());

  NumberConvert convert(
      std::make_unique<NumberConst>(175.0, metalib.units().get_unit(Units::cm())),
      metalib.units(), Units::mm());
  EXPECT_TRUE(convert.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(convert.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(convert.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(convert.value(Scope::null()), 1750.0);
  EXPECT_EQ(convert.dimension(Scope::null()), Units::mm());

  NumberDim dim(
      std::make_unique<NumberConst>(175.0, metalib.units().get_unit(Units::cm())),
      Units::mm(), false);
  EXPECT_TRUE(dim.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(dim.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(dim.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(dim.value(Scope::null()), 175.0);
  EXPECT_EQ(dim.dimension(Scope::null()), Units::mm());
}

TEST(NumberExposureTest, NumberIdentityCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "identity");
  std::unique_ptr<FrameModel> value = clone_model(library, "const");
  value->set("value", 175.0, Units::cm());
  frame->set("value", *value);
  frame->set("dimension", Units::mm());

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "identity");
  NumberIdentity number(block);

  EXPECT_TRUE(number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(number.value(Scope::null()), 1750.0);
  EXPECT_EQ(number.dimension(Scope::null()), Units::mm());
}

TEST(NumberExposureTest, NumberPlfHasDirectConstructor) {
  register_test_models();
  Metalib metalib(load_test_frame);
  PLF plf;
  plf.add(0.0, 0.0);
  plf.add(2.0, 40.0);

  NumberPLF number(
      std::make_unique<NumberConst>(1.0, metalib.units().get_unit(Units::cm())),
      Units::cm(), Units::mm(), plf);

  EXPECT_TRUE(number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), Scope::null(), Treelog::null()));
  number.tick(metalib.units(), Scope::null(), Treelog::null());
  EXPECT_FALSE(number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(number.value(Scope::null()), 20.0);
  EXPECT_EQ(number.dimension(Scope::null()), Units::mm());
}

TEST(NumberExposureTest, NumberPlfCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "plf");
  std::unique_ptr<FrameModel> operand = clone_model(library, "const");
  operand->set("value", 1.0, Units::cm());
  frame->set("operand", *operand);
  frame->set("domain", Units::cm());
  frame->set("range", Units::mm());
  std::vector<boost::shared_ptr<const FrameSubmodel>> points;
  points.push_back(make_number_plf_point(0.0, Units::cm(), 0.0, Units::mm()));
  points.push_back(make_number_plf_point(2.0, Units::cm(), 40.0, Units::mm()));
  frame->set("points", points);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "plf");
  NumberPLF number(block);

  EXPECT_TRUE(number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(number.check(metalib.units(), Scope::null(), Treelog::null()));
  number.tick(metalib.units(), Scope::null(), Treelog::null());
  EXPECT_FALSE(number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(number.value(Scope::null()), 20.0);
  EXPECT_EQ(number.dimension(Scope::null()), Units::mm());
}

TEST(NumberExposureTest, NumberUnaryArithmeticClassesHaveDirectConstructors) {
  register_test_models();
  Metalib metalib(load_test_frame);

  NumberLog10 log10_number(
      std::make_unique<NumberConst>(100.0, metalib.units().get_unit(Units::cm())));
  EXPECT_TRUE(log10_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(log10_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_FALSE(log10_number.missing(Scope::null()));
  EXPECT_DOUBLE_EQ(log10_number.value(Scope::null()), 2.0);

  NumberLn ln_number(
      std::make_unique<NumberConst>(std::exp(1.0), metalib.units().get_unit(Units::cm())));
  EXPECT_NEAR(ln_number.value(Scope::null()), 1.0, 1e-12);

  NumberExp exp_number(
      std::make_unique<NumberConst>(1.0, metalib.units().get_unit(Units::cm())));
  EXPECT_NEAR(exp_number.value(Scope::null()), std::exp(1.0), 1e-12);

  NumberSqrt sqrt_number(
      std::make_unique<NumberConst>(9.0, metalib.units().get_unit(Units::cm())));
  EXPECT_DOUBLE_EQ(sqrt_number.value(Scope::null()), 3.0);

  NumberSqr sqr_number(
      std::make_unique<NumberConst>(3.0, metalib.units().get_unit(Units::cm())));
  EXPECT_DOUBLE_EQ(sqr_number.value(Scope::null()), 9.0);
  EXPECT_EQ(sqr_number.dimension(Scope::null()),
            Units::multiply(Units::cm(), Units::cm()));
}

TEST(NumberExposureTest, NumberUnaryArithmeticClassesCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);

  std::unique_ptr<FrameModel> log10_frame = clone_model(library, "log10");
  std::unique_ptr<FrameModel> log10_operand = clone_model(library, "const");
  log10_operand->set("value", 100.0, Units::cm());
  log10_frame->set("operand", *log10_operand);

  std::unique_ptr<FrameModel> sqr_frame = clone_model(library, "sqr");
  std::unique_ptr<FrameModel> sqr_operand = clone_model(library, "const");
  sqr_operand->set("value", 3.0, Units::cm());
  sqr_frame->set("operand", *sqr_operand);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel log10_block(context, *log10_frame, "log10");
  BlockModel sqr_block(context, *sqr_frame, "sqr");
  NumberLog10 log10_number(log10_block);
  NumberSqr sqr_number(sqr_block);

  EXPECT_TRUE(log10_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(log10_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_DOUBLE_EQ(log10_number.value(Scope::null()), 2.0);

  EXPECT_TRUE(sqr_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(sqr_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_DOUBLE_EQ(sqr_number.value(Scope::null()), 9.0);
  EXPECT_EQ(sqr_number.dimension(Scope::null()),
            Units::multiply(Units::cm(), Units::cm()));
}

TEST(NumberExposureTest, NumberPowAndSharedOperandClassesHaveDirectConstructors) {
  register_test_models();
  Metalib metalib(load_test_frame);

  NumberPow pow_number(
      std::make_unique<NumberConst>(4.0, metalib.units().get_unit(Units::cm())),
      std::make_unique<NumberConst>(0.5, metalib.units().get_unit(Attribute::None())));
  EXPECT_TRUE(pow_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(pow_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_DOUBLE_EQ(pow_number.value(Scope::null()), 2.0);

  NumberMax max_number(make_const_operands(metalib, {3.0, 7.0, 5.0}));
  EXPECT_DOUBLE_EQ(max_number.value(Scope::null()), 7.0);
  EXPECT_EQ(max_number.dimension(Scope::null()), Units::cm());

  NumberMin min_number(make_const_operands(metalib, {3.0, 7.0, 5.0}));
  EXPECT_DOUBLE_EQ(min_number.value(Scope::null()), 3.0);

  NumberProduct product_number(make_const_operands(metalib, {2.0, 3.0}));
  EXPECT_DOUBLE_EQ(product_number.value(Scope::null()), 6.0);

  NumberSum sum_number(make_const_operands(metalib, {2.0, 3.0, 5.0}));
  EXPECT_DOUBLE_EQ(sum_number.value(Scope::null()), 10.0);
  EXPECT_EQ(sum_number.dimension(Scope::null()), Units::cm());

  NumberSubtract subtract_number(make_const_operands(metalib, {10.0, 3.0, 2.0}));
  EXPECT_DOUBLE_EQ(subtract_number.value(Scope::null()), 5.0);

  NumberDivide divide_number(make_const_operands(metalib, {12.0, 3.0, 2.0}));
  EXPECT_DOUBLE_EQ(divide_number.value(Scope::null()), 2.0);
}

TEST(NumberExposureTest, NumberPowAndSharedOperandClassesCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Number::component);

  std::unique_ptr<FrameModel> pow_frame = clone_model(library, "pow");
  std::unique_ptr<FrameModel> pow_base = clone_model(library, "const");
  std::unique_ptr<FrameModel> pow_exponent = clone_model(library, "const");
  pow_base->set("value", 4.0, Units::cm());
  pow_exponent->set("value", 0.5, Attribute::None());
  pow_frame->set("base", *pow_base);
  pow_frame->set("exponent", *pow_exponent);

  std::unique_ptr<FrameModel> max_frame = clone_model(library, "max");
  std::vector<boost::shared_ptr<const FrameModel>> max_operands;
  max_operands.push_back(make_number_const_frame(library, 3.0, Units::cm()));
  max_operands.push_back(make_number_const_frame(library, 7.0, Units::cm()));
  max_operands.push_back(make_number_const_frame(library, 5.0, Units::cm()));
  max_frame->set("operands", max_operands);

  std::unique_ptr<FrameModel> divide_frame = clone_model(library, "/");
  std::vector<boost::shared_ptr<const FrameModel>> divide_operands;
  divide_operands.push_back(make_number_const_frame(library, 12.0, Units::cm()));
  divide_operands.push_back(make_number_const_frame(library, 3.0, Attribute::None()));
  divide_operands.push_back(make_number_const_frame(library, 2.0, Attribute::None()));
  divide_frame->set("operands", divide_operands);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel pow_block(context, *pow_frame, "pow");
  BlockModel max_block(context, *max_frame, "max");
  BlockModel divide_block(context, *divide_frame, "/");
  NumberPow pow_number(pow_block);
  NumberMax max_number(max_block);
  NumberDivide divide_number(divide_block);

  EXPECT_TRUE(pow_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(pow_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_DOUBLE_EQ(pow_number.value(Scope::null()), 2.0);

  EXPECT_TRUE(max_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(max_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_DOUBLE_EQ(max_number.value(Scope::null()), 7.0);
  EXPECT_EQ(max_number.dimension(Scope::null()), Units::cm());

  EXPECT_TRUE(divide_number.initialize(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_TRUE(divide_number.check(metalib.units(), Scope::null(), Treelog::null()));
  EXPECT_DOUBLE_EQ(divide_number.value(Scope::null()), 2.0);
}
