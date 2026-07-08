#include <set>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/frame_model.h"
#include "object_model/function.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/object_model_registration_internal.h"
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
  register_function_models();
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
}

TEST(FunctionRegistrationTest, FunctionModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Function::component);

  EXPECT_TRUE(library.check("const"));
  EXPECT_TRUE(library.check("plf"));
  EXPECT_TRUE(library.is_derived_from("const", "const"));
  EXPECT_TRUE(library.is_derived_from("plf", "plf"));
  EXPECT_TRUE(library.is_derived_from("const", "component"));
  EXPECT_TRUE(library.is_derived_from("plf", "component"));
  EXPECT_EQ(library.base_model("const"), symbol("component"));
  EXPECT_EQ(library.base_model("plf"), symbol("component"));

  const FrameModel& const_model = library.model("const");
  const FrameModel& plf_model = library.model("plf");
  EXPECT_EQ(const_model.type_name(), symbol("const"));
  EXPECT_EQ(const_model.base_name(), symbol("component"));
  EXPECT_EQ(plf_model.type_name(), symbol("plf"));
  EXPECT_EQ(plf_model.base_name(), symbol("component"));
}

TEST(FunctionRegistrationTest, FunctionComponentSymbolIsStable) {
  EXPECT_EQ(symbol(Function::component), symbol("function"));
}

TEST(FunctionExposureTest, ConcreteFunctionClassesArePublicTypes) {
  EXPECT_TRUE((std::is_base_of<Function, FunctionConst>::value));
  EXPECT_TRUE((std::is_base_of<Function, FunctionPLF>::value));
  EXPECT_TRUE((std::is_constructible<FunctionConst, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<FunctionPLF, const BlockModel&>::value));
}
