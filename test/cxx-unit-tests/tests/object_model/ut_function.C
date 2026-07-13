#include <memory>
#include <set>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#ifdef BUILD_PYTHON
#include <pybind11/embed.h>
#endif

#include "object_model/block_model.h"
#include "object_model/block_top.h"
#include "object_model/frame_model.h"
#include "object_model/function.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/units.h"

namespace {

#ifdef BUILD_PYTHON
pybind11::scoped_interpreter python_interpreter;
#endif

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
  register_function_models();
#ifdef BUILD_PYTHON
  register_function_python_models();
#endif
}

}  // namespace

TEST(FunctionRegistrationTest, FunctionLibraryContainsExpectedModels) {
  register_test_models();
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Function::component));
  const Library& library = metalib.library(Function::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("const"));
  EXPECT_TRUE(entries.count("plf"));
#ifdef BUILD_PYTHON
  EXPECT_TRUE(entries.count("Python"));
#endif
}

TEST(FunctionRegistrationTest, FunctionModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Function::component);

  EXPECT_TRUE(library.check("const"));
  EXPECT_TRUE(library.check("plf"));
#ifdef BUILD_PYTHON
  EXPECT_TRUE(library.check("Python"));
#endif
  EXPECT_TRUE(library.is_derived_from("const", "const"));
  EXPECT_TRUE(library.is_derived_from("plf", "plf"));
  EXPECT_TRUE(library.is_derived_from("const", "component"));
  EXPECT_TRUE(library.is_derived_from("plf", "component"));
#ifdef BUILD_PYTHON
  EXPECT_TRUE(library.is_derived_from("Python", "component"));
#endif
  EXPECT_EQ(library.base_model("const"), symbol("component"));
  EXPECT_EQ(library.base_model("plf"), symbol("component"));
#ifdef BUILD_PYTHON
  EXPECT_EQ(library.base_model("Python"), symbol("component"));
#endif

  const FrameModel& const_model = library.model("const");
  const FrameModel& plf_model = library.model("plf");
  EXPECT_EQ(const_model.type_name(), symbol("const"));
  EXPECT_EQ(const_model.base_name(), symbol("component"));
  EXPECT_EQ(plf_model.type_name(), symbol("plf"));
  EXPECT_EQ(plf_model.base_name(), symbol("component"));
#ifdef BUILD_PYTHON
  const FrameModel& python_model = library.model("Python");
  EXPECT_EQ(python_model.type_name(), symbol("Python"));
  EXPECT_EQ(python_model.base_name(), symbol("component"));
#endif
}

TEST(FunctionRegistrationTest, FunctionComponentSymbolIsStable) {
  EXPECT_EQ(symbol(Function::component), symbol("function"));
}

TEST(FunctionExposureTest, ConcreteFunctionClassesArePublicTypes) {
  EXPECT_TRUE((std::is_base_of<Function, FunctionConst>::value));
  EXPECT_TRUE((std::is_base_of<Function, FunctionPLF>::value));
  EXPECT_TRUE((std::is_constructible<FunctionConst, double>::value));
  EXPECT_TRUE((std::is_constructible<FunctionPLF, const PLF&>::value));
  EXPECT_TRUE((std::is_constructible<FunctionConst, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<FunctionPLF, const BlockModel&>::value));
#ifdef BUILD_PYTHON
  EXPECT_TRUE((std::is_base_of<Function, FunctionPython>::value));
  EXPECT_TRUE((std::is_constructible<FunctionPython, symbol, symbol, symbol, symbol>::value));
  EXPECT_TRUE((std::is_constructible<FunctionPython, const BlockModel&>::value));
#endif
}

TEST(FunctionExposureTest, FunctionConstHasDirectValueConstructor) {
  FunctionConst function(4.25);

  EXPECT_DOUBLE_EQ(function.value(-10.0), 4.25);
  EXPECT_DOUBLE_EQ(function.value(10.0), 4.25);
}

TEST(FunctionExposureTest, FunctionPLFHasDirectPlfConstructor) {
  PLF plf;
  plf.add(0.0, 0.0);
  plf.add(2.0, 4.0);

  FunctionPLF function(plf);

  EXPECT_DOUBLE_EQ(function.value(0.0), 0.0);
  EXPECT_DOUBLE_EQ(function.value(1.0), 2.0);
  EXPECT_DOUBLE_EQ(function.value(2.0), 4.0);
}

TEST(FunctionExposureTest, FunctionConstCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Function::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "const");
  frame->set("value", 4.25);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "const");
  FunctionConst function(block);

  EXPECT_DOUBLE_EQ(function.value(-10.0), 4.25);
  EXPECT_DOUBLE_EQ(function.value(10.0), 4.25);
}

TEST(FunctionExposureTest, FunctionPLFCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Function::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "plf");

  PLF plf;
  plf.add(0.0, 0.0);
  plf.add(2.0, 4.0);
  frame->set("plf", plf);

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "plf");
  FunctionPLF function(block);

  EXPECT_DOUBLE_EQ(function.value(0.0), 0.0);
  EXPECT_DOUBLE_EQ(function.value(1.0), 2.0);
  EXPECT_DOUBLE_EQ(function.value(2.0), 4.0);
}

#ifdef BUILD_PYTHON
TEST(FunctionExposureTest, FunctionPythonHasDirectConstructor) {
  FunctionPython function("math", "sqrt", "none", "none");

  EXPECT_DOUBLE_EQ(function.value(4.0), 2.0);
  EXPECT_DOUBLE_EQ(function.value(9.0), 3.0);
}

TEST(FunctionExposureTest, FunctionPythonCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Function::component);
  std::unique_ptr<FrameModel> frame = clone_model(library, "Python");
  frame->set("module", "math");
  frame->set("name", "sqrt");
  frame->set("domain", "none");
  frame->set("range", "none");

  BlockTop context(metalib, Treelog::null(), metalib);
  BlockModel block(context, *frame, "Python");
  FunctionPython function(block);

  EXPECT_DOUBLE_EQ(function.value(4.0), 2.0);
  EXPECT_DOUBLE_EQ(function.value(9.0), 3.0);
}
#endif
