#include <gtest/gtest.h>

#include <set>

#include "object_model/attribute.h"
#include "util/scope.h"

namespace {

class DummyScope : public Scope {
public:
  void entries(std::set<symbol>& result) const override { result.insert("x"); }
  Attribute::type lookup(symbol name) const override {
    return name == symbol("x") ? Attribute::Number : Attribute::Error;
  }
  symbol dimension(symbol) const override { return "kg"; }
  symbol description(symbol) const override { return "value"; }
  bool check(symbol name) const override { return name == symbol("x"); }
  double number(symbol) const override { return 42.0; }
};

}  // namespace

TEST(ScopeTest, NullScopeIsSingletonAndEmpty) {
  Scope& one = Scope::null();
  Scope& two = Scope::null();
  std::set<symbol> entries;

  EXPECT_EQ(&one, &two);
  EXPECT_EQ(one.title(), Attribute::Unknown());
  one.entries(entries);
  EXPECT_TRUE(entries.empty());
  EXPECT_EQ(one.lookup("x"), Attribute::Error);
  EXPECT_FALSE(one.check("x"));
  EXPECT_FALSE(one.can_extract_as("x", Attribute::Number));
}

TEST(ScopeTest, DefaultHelpersUseLookupAndSingletonSizes) {
  DummyScope scope;

  EXPECT_EQ(scope.title(), Attribute::Unknown());
  EXPECT_TRUE(scope.can_extract_as("x", Attribute::Number));
  EXPECT_FALSE(scope.can_extract_as("x", Attribute::String));
  EXPECT_FALSE(scope.can_extract_as("missing", Attribute::Number));
  EXPECT_EQ(scope.type_size("x"), Attribute::Singleton);
  EXPECT_EQ(scope.value_size("x"), Attribute::Singleton);
  EXPECT_EQ(scope.dimension("x"), symbol("kg"));
  EXPECT_EQ(scope.description("x"), symbol("value"));
  EXPECT_EQ(scope.number("x"), 42.0);
}
