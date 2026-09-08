#include <set>

#include <gtest/gtest.h>

#include "object_model/attribute.h"
#include "util/scope_id.h"
#include "util/scope_multi.h"

TEST(ScopeIDTest, TracksValueDimensionAndDescriptionForSingleIdentifier) {
  ScopeID scope("temperature", "K");
  std::set<symbol> entries;

  scope.entries(entries);
  EXPECT_EQ(entries.size(), 1U);
  EXPECT_TRUE(entries.find("temperature") != entries.end());

  EXPECT_EQ(scope.lookup("temperature"), Attribute::Number);
  EXPECT_EQ(scope.lookup("missing"), Attribute::Error);
  EXPECT_FALSE(scope.check("temperature"));

  scope.set("temperature", 273.15);
  EXPECT_TRUE(scope.check("temperature"));
  EXPECT_DOUBLE_EQ(scope.number("temperature"), 273.15);
  EXPECT_EQ(scope.dimension("temperature"), symbol("K"));
  EXPECT_EQ(scope.description("temperature"),
            symbol("Use 'temperature' as a free variable"));

  scope.set_dimension("temperature", "degC");
  EXPECT_EQ(scope.dimension("temperature"), symbol("degC"));
}

TEST(ScopeMultiTest, MergesEntriesAndResolvesValuesFromMatchingScope) {
  ScopeID first("x", "kg");
  first.set("x", 1.5);

  ScopeID second("y", "m");
  second.set("y", 2.0);

  ScopeMulti scope(first, second);
  std::set<symbol> entries;
  scope.entries(entries);

  EXPECT_EQ(entries.size(), 2U);
  EXPECT_TRUE(entries.find("x") != entries.end());
  EXPECT_TRUE(entries.find("y") != entries.end());

  EXPECT_EQ(scope.lookup("x"), Attribute::Number);
  EXPECT_EQ(scope.lookup("y"), Attribute::Number);
  EXPECT_EQ(scope.lookup("missing"), Attribute::Error);

  EXPECT_TRUE(scope.check("x"));
  EXPECT_TRUE(scope.check("y"));
  EXPECT_FALSE(scope.check("missing"));

  EXPECT_DOUBLE_EQ(scope.number("x"), 1.5);
  EXPECT_DOUBLE_EQ(scope.number("y"), 2.0);
  EXPECT_EQ(scope.dimension("x"), symbol("kg"));
  EXPECT_EQ(scope.dimension("y"), symbol("m"));
  EXPECT_EQ(scope.description("x"), symbol("Use 'x' as a free variable"));
  EXPECT_EQ(scope.description("y"), symbol("Use 'y' as a free variable"));
}

TEST(ScopeMultiTest, FirstMatchingScopeWinsForOverlappingIdentifiers) {
  ScopeID first("shared", "kg");
  first.set("shared", 1.0);

  ScopeID second("shared", "g");
  second.set("shared", 1000.0);

  ScopeMulti scope(first, second);

  EXPECT_TRUE(scope.check("shared"));
  EXPECT_DOUBLE_EQ(scope.number("shared"), 1.0);
  EXPECT_EQ(scope.dimension("shared"), symbol("kg"));
  EXPECT_EQ(scope.description("shared"), symbol("Use 'shared' as a free variable"));
}
