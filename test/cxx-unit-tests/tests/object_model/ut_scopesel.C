#include <memory>
#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/frame_model.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/treelog_text.h"
#include "object_model/units.h"
#include "util/scopesel.h"

namespace {

void load_test_frame(Frame& frame) {
  Units::load_syntax(frame);
}

std::set<symbol> library_entries(const Library& library) {
  std::vector<symbol> entries;
  library.entries(entries);
  return std::set<symbol>(entries.begin(), entries.end());
}

bool frame_is_valid(const Metalib& metalib, const Frame& frame) {
  TreelogString log;
  return frame.check(metalib, log);
}

std::string frame_errors(const Metalib& metalib, const Frame& frame) {
  TreelogString log;
  static_cast<void>(frame.check(metalib, log));
  return log.str();
}

}  // namespace

TEST(ScopeselRegistrationTest, LibraryContainsExpectedModels) {
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Scopesel::component));
  const Library& library = metalib.library(Scopesel::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("name"));
  EXPECT_TRUE(entries.count("null"));
  EXPECT_TRUE(entries.count("multi"));
}

TEST(ScopeselRegistrationTest, SelectorModelsExposeCurrentMetadata) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Scopesel::component);

  EXPECT_TRUE(library.is_derived_from("name", "component"));
  EXPECT_TRUE(library.is_derived_from("null", "component"));
  EXPECT_TRUE(library.is_derived_from("multi", "component"));
  EXPECT_EQ(library.base_model("name"), symbol("component"));
  EXPECT_EQ(library.base_model("null"), symbol("component"));
  EXPECT_EQ(library.base_model("multi"), symbol("component"));

  const FrameModel& name_model = library.model("name");
  const FrameModel& null_model = library.model("null");
  const FrameModel& multi_model = library.model("multi");

  EXPECT_EQ(name_model.type_name(), symbol("name"));
  EXPECT_EQ(name_model.lookup("name"), Attribute::String);
  EXPECT_TRUE(name_model.is_const("name"));
  EXPECT_EQ(name_model.type_size("name"), Attribute::Singleton);
  EXPECT_EQ(name_model.order_index("name"), 0);

  EXPECT_EQ(null_model.type_name(), symbol("null"));
  EXPECT_EQ(null_model.lookup("name"), Attribute::Error);
  EXPECT_TRUE(frame_is_valid(metalib, null_model));

  EXPECT_EQ(multi_model.type_name(), symbol("multi"));
  EXPECT_EQ(multi_model.lookup("name"), Attribute::String);
  EXPECT_TRUE(multi_model.is_const("name"));
  EXPECT_EQ(multi_model.type_size("name"), Attribute::Variable);
  EXPECT_EQ(multi_model.order_index("name"), 0);
}

TEST(ScopeselRegistrationTest, MultiSelectorRejectsDuplicateNames) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Scopesel::component);

  std::unique_ptr<FrameModel> valid(&library.model("multi").clone());
  valid->set_strings("name", "weather", "soil");
  EXPECT_TRUE(frame_is_valid(metalib, *valid));

  std::unique_ptr<FrameModel> duplicate(&library.model("multi").clone());
  duplicate->set_strings("name", "weather", "weather");
  EXPECT_FALSE(frame_is_valid(metalib, *duplicate));
  EXPECT_FALSE(frame_errors(metalib, *duplicate).empty());
}

TEST(ScopeselRegistrationTest, ComponentSymbolIsStable) {
  EXPECT_EQ(symbol(Scopesel::component), symbol("scopesel"));
}
