#include <vector>

#include <gtest/gtest.h>

#include "object_model/check.h"
#include "object_model/treelog_text.h"
#include "object_model/type.h"

namespace {

void dummy_load_syntax(Frame&) {}

}  // namespace

TEST(TypeTest, NumberTypesExposeCategoryDimensionAndValidation) {
  TypeNumber number_type(Attribute::OptionalConst, 3, "kg", Check::positive(), "mass");
  const Type& as_type = number_type;
  TreelogString ok_log;
  TreelogString bad_log;

  EXPECT_EQ(as_type.type(), Attribute::Number);
  EXPECT_EQ(as_type.category(), Attribute::OptionalConst);
  EXPECT_TRUE(as_type.is_const());
  EXPECT_TRUE(as_type.is_optional());
  EXPECT_FALSE(as_type.is_state());
  EXPECT_FALSE(as_type.is_log());
  EXPECT_EQ(as_type.size(), 3);
  EXPECT_EQ(as_type.description(), symbol("mass"));
  EXPECT_EQ(as_type.dimension(), symbol("kg"));
  EXPECT_TRUE(as_type.verify(1.0, ok_log));
  EXPECT_TRUE(ok_log.str().empty());
  EXPECT_FALSE(as_type.verify(0.0, bad_log));
  EXPECT_FALSE(bad_log.str().empty());
}

TEST(TypeTest, NumberTypeCitationsAreAvailableThroughBaseInterface) {
  const std::vector<symbol> citations{symbol("ref-1"), symbol("ref-2")};
  TypeNumberCite number_type(Attribute::Const, Attribute::Singleton, "m", Check::none(),
                             "length", citations);
  const Type& as_type = number_type;

  ASSERT_EQ(as_type.cite().size(), citations.size());
  EXPECT_EQ(as_type.cite()[0], citations[0]);
  EXPECT_EQ(as_type.cite()[1], citations[1]);
}

TEST(TypeTest, SubmodelTypeExposesLoadSyntaxAndStateFlags) {
  TypeSubmodel submodel_type(Attribute::State, Attribute::Singleton, dummy_load_syntax,
                             "submodel");
  const Type& as_type = submodel_type;

  EXPECT_EQ(as_type.type(), Attribute::Submodel);
  EXPECT_TRUE(as_type.is_mandatory());
  EXPECT_TRUE(as_type.is_state());
  EXPECT_FALSE(as_type.is_optional());
  EXPECT_EQ(as_type.load_syntax(), dummy_load_syntax);
}

TEST(TypeTest, PLFTypeUsesDomainAsDimensionAndDelegatesChecks) {
  TypePLF plf_type(Attribute::LogOnly, 2, "day", "kg", Check::fraction(), "curve");
  const Type& as_type = plf_type;
  TreelogString ok_log;
  TreelogString bad_log;

  EXPECT_EQ(as_type.type(), Attribute::PLF);
  EXPECT_TRUE(as_type.is_log());
  EXPECT_FALSE(as_type.is_const());
  EXPECT_EQ(as_type.dimension(), symbol("day"));
  EXPECT_EQ(as_type.domain(), symbol("day"));
  EXPECT_EQ(as_type.range(), symbol("kg"));
  EXPECT_TRUE(as_type.verify(0.25, ok_log));
  EXPECT_FALSE(as_type.verify(1.5, bad_log));
}

TEST(TypeTest, TextBooleanAndIntegerTypesReportTheirKinds) {
  TypeBoolean bool_type(Attribute::Const, Attribute::Singleton, "flag");
  TypeString string_type(Attribute::OptionalConst, Attribute::Variable, "name");
  TypeText text_type(Attribute::OptionalConst, Attribute::Variable, "text");
  TypeInteger integer_type(Attribute::State, 4, "count");

  EXPECT_EQ(static_cast<const Type&>(bool_type).type(), Attribute::Boolean);
  EXPECT_EQ(static_cast<const Type&>(string_type).type(), Attribute::String);
  EXPECT_FALSE(static_cast<const Type&>(string_type).is_text());
  EXPECT_EQ(static_cast<const Type&>(text_type).type(), Attribute::String);
  EXPECT_TRUE(static_cast<const Type&>(text_type).is_text());
  EXPECT_EQ(static_cast<const Type&>(integer_type).type(), Attribute::Integer);
  EXPECT_TRUE(static_cast<const Type&>(integer_type).is_state());
}

TEST(TypeTest, ModelAndFunctionTypesExposeComponentsAndRanges) {
  TypeModel model_type(Attribute::OptionalState, Attribute::Variable, "crop", "crop model");
  TypeFunction function_type("day", "kg", "response");

  const Type& as_model = model_type;
  const Type& as_function = function_type;

  EXPECT_EQ(as_model.type(), Attribute::Model);
  EXPECT_EQ(as_model.component(), symbol("crop"));
  EXPECT_TRUE(as_model.is_optional());
  EXPECT_TRUE(as_model.is_state());

  EXPECT_EQ(as_function.type(), Attribute::Function);
  EXPECT_EQ(as_function.component(), symbol("function"));
  EXPECT_EQ(as_function.dimension(), symbol("day"));
  EXPECT_EQ(as_function.domain(), symbol("day"));
  EXPECT_EQ(as_function.range(), symbol("kg"));
  EXPECT_TRUE(as_function.is_const());
  EXPECT_EQ(as_function.size(), Attribute::Singleton);
}
