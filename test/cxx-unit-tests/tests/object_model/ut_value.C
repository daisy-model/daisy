#include <memory>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>

#include "object_model/attribute.h"
#include "object_model/plf.h"
#include "object_model/value.h"

TEST(ValueTest, BaseDefaultsAreSafeForMetadata) {
  ValueNumber value(1.5);
  const Value& as_value = value;

  EXPECT_EQ(as_value.size(), Attribute::Singleton);
  EXPECT_TRUE(as_value.cite().empty());
  EXPECT_EQ(as_value.description(), Attribute::None());
  EXPECT_FALSE(as_value.is_reference());
}

TEST(ValueTest, NumberDescriptionAndCitationArePreserved) {
  const std::vector<symbol> citations{symbol("A"), symbol("B")};
  ValueNumberCite value(2.5, "description", citations);
  const Value& as_value = value;

  EXPECT_DOUBLE_EQ(as_value.number(), 2.5);
  EXPECT_EQ(as_value.description(), symbol("description"));
  ASSERT_EQ(as_value.cite().size(), citations.size());
  EXPECT_EQ(as_value.cite()[0], citations[0]);
  EXPECT_EQ(as_value.cite()[1], citations[1]);
}

TEST(ValueTest, ReferenceReportsNameAndThrowsOnNumberLookup) {
  ValueReference value("soil.water");
  const Value& as_value = value;

  EXPECT_EQ(as_value.size(), -1);
  EXPECT_TRUE(as_value.is_reference());
  EXPECT_EQ(as_value.name(), symbol("soil.water"));
  EXPECT_ANY_THROW(static_cast<void>(as_value.number()));
}

TEST(ValueTest, ScalarStringBooleanAndIntegerAccessorsWork) {
  ValueScalar scalar(3.25, "kg");
  ValueString string_value("hello");
  ValueBoolean bool_value(true);
  ValueInteger int_value(7);

  EXPECT_DOUBLE_EQ(static_cast<const Value&>(scalar).number(), 3.25);
  EXPECT_EQ(static_cast<const Value&>(scalar).name(), symbol("kg"));
  EXPECT_EQ(static_cast<const Value&>(string_value).name(), symbol("hello"));
  EXPECT_TRUE(static_cast<const Value&>(bool_value).flag());
  EXPECT_EQ(static_cast<const Value&>(int_value).integer(), 7);
}

TEST(ValueTest, PLFAndModelPointersRoundTrip) {
  boost::shared_ptr<PLF> mutable_plf(new PLF());
  mutable_plf->add(0.0, 1.0);
  mutable_plf->add(1.0, 2.0);
  const boost::shared_ptr<const PLF> plf = mutable_plf;
  ValuePLFCite plf_value(plf, "curve", {symbol("citation")});

  const boost::shared_ptr<const FrameModel> model;
  const boost::shared_ptr<const FrameSubmodel> submodel;
  ValueModel model_value(model);
  ValueSubmodel submodel_value(submodel);

  EXPECT_EQ(static_cast<const Value&>(plf_value).plf().get(), plf.get());
  EXPECT_EQ(static_cast<const Value&>(plf_value).description(), symbol("curve"));
  ASSERT_EQ(static_cast<const Value&>(plf_value).cite().size(), 1U);
  EXPECT_EQ(static_cast<const Value&>(plf_value).cite()[0], symbol("citation"));
  EXPECT_EQ(static_cast<const Value&>(model_value).model().get(), nullptr);
  EXPECT_EQ(static_cast<const Value&>(submodel_value).submodel().get(), nullptr);
}

TEST(ValueTest, SequenceValuesExposeUnderlyingStorage) {
  const std::vector<double> numbers{1.0, 2.0, 3.0};
  const std::vector<symbol> names{symbol("a"), symbol("b")};
  const std::vector<bool> flags{true, false, true};
  const std::vector<int> integers{4, 5};

  boost::shared_ptr<PLF> mutable_plf(new PLF());
  mutable_plf->add(0.0, 0.0);
  const boost::shared_ptr<const PLF> plf = mutable_plf;
  const std::vector<boost::shared_ptr<const PLF>> plfs{plf};
  const std::vector<boost::shared_ptr<const FrameModel>> models(2);
  const std::vector<boost::shared_ptr<const FrameSubmodel>> submodels(3);

  ValueNumberSeq number_values(numbers);
  ValueStringSeq name_values(names);
  ValueBooleanSeq flag_values(flags);
  ValueIntegerSeq integer_values(integers);
  ValuePLFSeq plf_values(plfs);
  ValueModelSeq model_values(models);
  ValueSubmodelSeq submodel_values(submodels);

  EXPECT_EQ(static_cast<const Value&>(number_values).size(), 3);
  EXPECT_EQ(static_cast<const Value&>(name_values).size(), 2);
  EXPECT_EQ(static_cast<const Value&>(flag_values).size(), 3);
  EXPECT_EQ(static_cast<const Value&>(integer_values).size(), 2);
  EXPECT_EQ(static_cast<const Value&>(plf_values).size(), 1);
  EXPECT_EQ(static_cast<const Value&>(model_values).size(), 2);
  EXPECT_EQ(static_cast<const Value&>(submodel_values).size(), 3);

  EXPECT_EQ(static_cast<const Value&>(number_values).number_sequence(), numbers);
  EXPECT_EQ(static_cast<const Value&>(name_values).name_sequence(), names);
  EXPECT_EQ(static_cast<const Value&>(flag_values).flag_sequence()[0], true);
  EXPECT_EQ(static_cast<const Value&>(flag_values).flag_sequence()[1], false);
  EXPECT_EQ(static_cast<const Value&>(flag_values).flag_sequence()[2], true);
  EXPECT_EQ(static_cast<const Value&>(integer_values).integer_sequence(), integers);
  EXPECT_EQ(static_cast<const Value&>(plf_values).plf_sequence()[0].get(), plf.get());
  EXPECT_EQ(static_cast<const Value&>(model_values).model_sequence()[0].get(), nullptr);
  EXPECT_EQ(static_cast<const Value&>(submodel_values).submodel_sequence()[0].get(), nullptr);
}
