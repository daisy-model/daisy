#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/frame_model.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/units.h"
#include "util/format.h"

namespace {

void load_test_frame(Frame& frame) {
  Units::load_syntax(frame);
}

std::set<symbol> library_entries(const Library& library) {
  std::vector<symbol> entries;
  library.entries(entries);
  return std::set<symbol>(entries.begin(), entries.end());
}

std::set<symbol> frame_entries(const Frame& frame) {
  std::set<symbol> entries;
  frame.entries(entries);
  return entries;
}

}  // namespace

TEST(FormatRegistrationTest, FormatLibraryContainsLaTeXModel) {
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(Format::component));
  const Library& library = metalib.library(Format::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("LaTeX"));
}

TEST(FormatRegistrationTest, LaTeXModelExposesCurrentMinimalFrameMetadata) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Format::component);
  const FrameModel& latex = library.model("LaTeX");
  const std::set<symbol> entries = frame_entries(latex);

  EXPECT_TRUE(library.check("LaTeX"));
  EXPECT_TRUE(library.is_derived_from("LaTeX", "component"));
  EXPECT_EQ(library.base_model("LaTeX"), symbol("component"));

  EXPECT_EQ(latex.type_name(), symbol("LaTeX"));
  EXPECT_EQ(latex.base_name(), symbol("component"));
  EXPECT_TRUE(entries.empty());
  EXPECT_EQ(latex.lookup("description"), Attribute::Error);
  EXPECT_EQ(latex.lookup("cite"), Attribute::Error);
}

TEST(FormatRegistrationTest, ComponentSymbolIsStable) {
  EXPECT_EQ(symbol(Format::component), symbol("format"));
}
