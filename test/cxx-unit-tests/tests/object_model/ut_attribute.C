#include <array>

#include <gtest/gtest.h>

#include "object_model/attribute.h"

TEST(AttributeTest, SizeNameCoversKnownAndUnknownSizes) {
  EXPECT_EQ(Attribute::size_name(Attribute::Singleton), symbol("singleton"));
  EXPECT_EQ(Attribute::size_name(Attribute::Variable), symbol("variable"));
  EXPECT_EQ(Attribute::size_name(Attribute::CanopyCells), symbol("canopy cells"));
  EXPECT_EQ(Attribute::size_name(Attribute::CanopyEdges), symbol("canopy edges"));
  EXPECT_EQ(Attribute::size_name(Attribute::SoilCells), symbol("soil cells"));
  EXPECT_EQ(Attribute::size_name(Attribute::SoilEdges), symbol("soil edges"));
  EXPECT_EQ(Attribute::size_name(Attribute::Unspecified), symbol("unspecified"));
  EXPECT_EQ(Attribute::size_name(7), symbol("[7]"));
}

TEST(AttributeTest, FlexibleSizeMatchesDynamicEntriesOnly) {
  EXPECT_FALSE(Attribute::flexible_size(Attribute::Singleton));
  EXPECT_TRUE(Attribute::flexible_size(Attribute::Variable));
  EXPECT_TRUE(Attribute::flexible_size(Attribute::CanopyCells));
  EXPECT_TRUE(Attribute::flexible_size(Attribute::CanopyEdges));
  EXPECT_TRUE(Attribute::flexible_size(Attribute::SoilCells));
  EXPECT_TRUE(Attribute::flexible_size(Attribute::SoilEdges));
  EXPECT_FALSE(Attribute::flexible_size(Attribute::Unspecified));
  EXPECT_FALSE(Attribute::flexible_size(3));
}

TEST(AttributeTest, SpecialSymbolsAreStable) {
  EXPECT_EQ(Attribute::Unknown(), symbol("<unknown>"));
  EXPECT_EQ(Attribute::None(), symbol("<none>"));
  EXPECT_EQ(Attribute::Fraction(), symbol("<fraction>"));
  EXPECT_EQ(Attribute::User(), symbol("<user>"));
}

TEST(AttributeTest, TypeNamesRoundTrip) {
  constexpr std::array<Attribute::type, 10> types{
    Attribute::Number,   Attribute::Submodel, Attribute::PLF,      Attribute::Boolean,
    Attribute::String,   Attribute::Integer,  Attribute::Model,    Attribute::Scalar,
    Attribute::Function, Attribute::Reference,
  };

  for (const Attribute::type type : types) {
    const symbol name = Attribute::type_name(type);
    EXPECT_EQ(Attribute::type_number(name), type);
  }

  EXPECT_EQ(Attribute::type_name(Attribute::Error), symbol("Error"));
  EXPECT_EQ(Attribute::type_number("no such type"), Attribute::Error);
}

TEST(AttributeTest, CategoryNamesRoundTrip) {
  constexpr std::array<Attribute::category, 5> categories{
    Attribute::Const,
    Attribute::State,
    Attribute::OptionalState,
    Attribute::OptionalConst,
    Attribute::LogOnly,
  };

  for (const Attribute::category category : categories) {
    const symbol name = Attribute::category_name(category);
    EXPECT_EQ(Attribute::category_number(name), static_cast<int>(category));
  }

  EXPECT_EQ(Attribute::category_number("no such category"), -1);
}
