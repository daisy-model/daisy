#include <memory>
#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/frame_model.h"
#include "object_model/function.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/parameter_types/boolean.h"
#include "object_model/units.h"

namespace {

void load_test_frame(Frame& frame) {
  Units::load_syntax(frame);
}

void dummy_doc_fun(Format&, const Metalib&, Treelog&, symbol) {}

std::set<symbol> library_entries(const Library& library) {
  std::vector<symbol> entries;
  library.entries(entries);
  return std::set<symbol>(entries.begin(), entries.end());
}

}  // namespace

TEST(LibraryTest, AncestorsTrackRegisteredInheritanceChains) {
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(Boolean::component);

  const std::set<symbol>& and_ancestors = library.ancestors("and");
  EXPECT_EQ(and_ancestors.size(), 3U);
  EXPECT_TRUE(and_ancestors.count("and"));
  EXPECT_TRUE(and_ancestors.count("operands"));
  EXPECT_TRUE(and_ancestors.count("component"));

  const std::set<symbol>& const_ancestors = metalib.library(Function::component).ancestors("const");
  EXPECT_EQ(const_ancestors.size(), 2U);
  EXPECT_TRUE(const_ancestors.count("const"));
  EXPECT_TRUE(const_ancestors.count("component"));
}

TEST(LibraryTest, ClonePreservesMetadataAndCanBeEditedIndependently) {
  Metalib metalib(load_test_frame);
  Library& original = metalib.library(Boolean::component);
  original.add_doc_fun(dummy_doc_fun);

  std::unique_ptr<Library> clone(original.clone());
  ASSERT_NE(clone, nullptr);

  EXPECT_EQ(clone->name(), original.name());
  EXPECT_EQ(clone->description(), original.description());
  ASSERT_EQ(clone->doc_funs().size(), original.doc_funs().size());
  EXPECT_EQ(clone->doc_funs().front(), dummy_doc_fun);

  clone->remove("and");
  EXPECT_FALSE(clone->check("and"));
  EXPECT_TRUE(original.check("and"));
}

TEST(LibraryTest, EntriesReflectRemovalOnClonedLibraryOnly) {
  Metalib metalib(load_test_frame);
  Library& original = metalib.library(Function::component);
  std::unique_ptr<Library> clone(original.clone());

  const std::set<symbol> before = library_entries(*clone);
  ASSERT_TRUE(before.count("const"));
  ASSERT_TRUE(before.count("plf"));

  clone->remove("plf");
  const std::set<symbol> after = library_entries(*clone);
  EXPECT_TRUE(after.count("const"));
  EXPECT_FALSE(after.count("plf"));
  EXPECT_TRUE(original.check("plf"));
}
