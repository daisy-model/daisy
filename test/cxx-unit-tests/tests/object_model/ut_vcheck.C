#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/check.h"
#include "object_model/frame.h"
#include "object_model/metalib.h"
#include "object_model/parameter_types/boolean.h"
#include "object_model/plf.h"
#include "object_model/treelog_text.h"
#include "object_model/units.h"
#include "object_model/vcheck.h"

namespace {

void load_test_frame(Frame& frame) {
  Units::load_syntax(frame);
}

class TestFrame : public Frame {
public:
  TestFrame() = default;
  Frame& clone() const override { return *new TestFrame(*this); }
};

bool frame_is_valid(const Metalib& metalib, const Frame& frame) {
  TreelogString log;
  return frame.check(metalib, log);
}

std::string frame_errors(const Metalib& metalib, const Frame& frame) {
  TreelogString log;
  static_cast<void>(frame.check(metalib, log));
  return log.str();
}

PLF make_plf(const std::vector<std::pair<double, double>>& points) {
  PLF plf;
  for (const auto& [x, y] : points) {
    plf.add(x, y);
  }
  return plf;
}

}  // namespace

TEST(VCheckTest, IntegerRangeValidatorsHonorBoundaries) {
  Metalib metalib(load_test_frame);

  TestFrame valid_month;
  valid_month.declare_integer("month", Attribute::Const, "month");
  valid_month.set_check("month", VCheck::valid_month());
  valid_month.set("month", 12);
  EXPECT_TRUE(frame_is_valid(metalib, valid_month));

  TestFrame invalid_month;
  invalid_month.declare_integer("month", Attribute::Const, "month");
  invalid_month.set_check("month", VCheck::valid_month());
  invalid_month.set("month", 13);
  EXPECT_FALSE(frame_is_valid(metalib, invalid_month));

  TestFrame valid_second;
  valid_second.declare_integer("second", Attribute::Const, "second");
  valid_second.set_check("second", VCheck::valid_second());
  valid_second.set("second", 59);
  EXPECT_TRUE(frame_is_valid(metalib, valid_second));
}

TEST(VCheckTest, SequenceOrderValidatorsWorkForNumberLists) {
  Metalib metalib(load_test_frame);

  TestFrame increasing;
  increasing.declare("series", "kg", Attribute::Const, Attribute::Variable, "series");
  increasing.set_check("series", VCheck::increasing());
  increasing.set("series", std::vector<double>{1.0, 2.0, 3.0});
  EXPECT_TRUE(frame_is_valid(metalib, increasing));

  TestFrame non_decreasing;
  non_decreasing.declare("series", "kg", Attribute::Const, Attribute::Variable, "series");
  non_decreasing.set_check("series", VCheck::non_decreasing());
  non_decreasing.set("series", std::vector<double>{1.0, 1.0, 2.0});
  EXPECT_TRUE(frame_is_valid(metalib, non_decreasing));

  TestFrame invalid_decreasing;
  invalid_decreasing.declare("series", "kg", Attribute::Const, Attribute::Variable, "series");
  invalid_decreasing.set_check("series", VCheck::decreasing());
  invalid_decreasing.set("series", std::vector<double>{3.0, 3.0, 1.0});
  EXPECT_FALSE(frame_is_valid(metalib, invalid_decreasing));
}

TEST(VCheckTest, CombinedSequenceChecksCatchWrongSumAndShortLists) {
  Metalib metalib(load_test_frame);
  const VCheck& sum_equal_1 = VCheck::sum_equal_1();
  const VCheck& min_size_1 = VCheck::min_size_1();
  const VCheck::All checks(sum_equal_1, min_size_1);

  TestFrame valid;
  valid.declare("fractions", "1", Attribute::Const, Attribute::Variable, "fractions");
  valid.set_check("fractions", checks);
  valid.set("fractions", std::vector<double>{0.25, 0.75});
  EXPECT_TRUE(frame_is_valid(metalib, valid));

  TestFrame wrong_sum;
  wrong_sum.declare("fractions", "1", Attribute::Const, Attribute::Variable, "fractions");
  wrong_sum.set_check("fractions", checks);
  wrong_sum.set("fractions", std::vector<double>{0.25, 0.5});
  EXPECT_FALSE(frame_is_valid(metalib, wrong_sum));

  TestFrame empty;
  empty.declare("fractions", "1", Attribute::Const, Attribute::Variable, "fractions");
  empty.set_check("fractions", checks);
  empty.set_empty("fractions");
  EXPECT_FALSE(frame_is_valid(metalib, empty));
}

TEST(VCheckTest, EnumAndCompatibleValidateStringValues) {
  Metalib metalib(load_test_frame);
  const VCheck::Enum enum_check("foo", "bar");
  const VCheck::Compatible compatible_check("K");

  TestFrame enum_frame;
  enum_frame.declare_string("choice", Attribute::Const, "choice");
  enum_frame.set_check("choice", enum_check);
  enum_frame.set("choice", "bar");
  EXPECT_TRUE(frame_is_valid(metalib, enum_frame));

  TestFrame invalid_enum_frame;
  invalid_enum_frame.declare_string("choice", Attribute::Const, "choice");
  invalid_enum_frame.set_check("choice", enum_check);
  invalid_enum_frame.set("choice", "baz");
  EXPECT_FALSE(frame_is_valid(metalib, invalid_enum_frame));

  TestFrame compatible_frame;
  compatible_frame.declare_string("unit", Attribute::Const, "unit");
  compatible_frame.set_check("unit", compatible_check);
  compatible_frame.set("unit", "dg C");
  EXPECT_TRUE(frame_is_valid(metalib, compatible_frame));

  TestFrame incompatible_frame;
  incompatible_frame.declare_string("unit", Attribute::Const, "unit");
  incompatible_frame.set_check("unit", compatible_check);
  incompatible_frame.set("unit", "banana");
  EXPECT_FALSE(frame_is_valid(metalib, incompatible_frame));
}

TEST(VCheckTest, LibraryAndBuildableChecksUseKnownModelsAndVariables) {
  Metalib metalib(load_test_frame);
  const VCheck::InLibrary in_library_check(Boolean::component);
  const VCheck::Buildable buildable_check(Boolean::component);

  TestFrame in_library;
  in_library.declare_string("bool_type", Attribute::Const, "bool type");
  in_library.set_check("bool_type", in_library_check);
  in_library.set("bool_type", "and");
  EXPECT_TRUE(frame_is_valid(metalib, in_library));

  TestFrame variable_reference;
  variable_reference.declare_string("bool_type", Attribute::Const, "bool type");
  variable_reference.set_check("bool_type", in_library_check);
  variable_reference.set("bool_type", "$runtime");
  EXPECT_TRUE(frame_is_valid(metalib, variable_reference));

  TestFrame invalid_library;
  invalid_library.declare_string("bool_type", Attribute::Const, "bool type");
  invalid_library.set_check("bool_type", in_library_check);
  invalid_library.set("bool_type", "not-a-model");
  EXPECT_FALSE(frame_is_valid(metalib, invalid_library));

  TestFrame buildable;
  buildable.declare_string("bool_type", Attribute::Const, "bool type");
  buildable.set_check("bool_type", buildable_check);
  buildable.set("bool_type", "and");
  EXPECT_TRUE(frame_is_valid(metalib, buildable));
}

TEST(VCheckTest, UniqueAndMultiSizeRejectDuplicateOrWrongSizedLists) {
  Metalib metalib(load_test_frame);
  const VCheck& unique_check = VCheck::unique();
  const VCheck::MultiSize multi_size_check(2, 3);
  const VCheck::All checks(unique_check, multi_size_check);

  TestFrame valid;
  valid.declare_string("names", Attribute::Const, Attribute::Variable, "names");
  valid.set_check("names", checks);
  valid.set_strings("names", "a", "b");
  EXPECT_TRUE(frame_is_valid(metalib, valid));

  TestFrame duplicate;
  duplicate.declare_string("names", Attribute::Const, Attribute::Variable, "names");
  duplicate.set_check("names", checks);
  duplicate.set_strings("names", "a", "a");
  EXPECT_FALSE(frame_is_valid(metalib, duplicate));

  TestFrame wrong_size;
  wrong_size.declare_string("names", Attribute::Const, Attribute::Variable, "names");
  wrong_size.set_check("names", checks);
  wrong_size.set_strings("names", "a");
  EXPECT_FALSE(frame_is_valid(metalib, wrong_size));
}

TEST(VCheckTest, PLFValidatorsCheckEndpointsAndFixedPoints) {
  Metalib metalib(load_test_frame);
  const VCheck& season_check = VCheck::season();
  const VCheck::FixedPoint fixed_point_check(100.0, 1.0);
  const VCheck::EndValue end_value_check(0.0);

  const PLF balanced = make_plf({{1.0, 0.0}, {100.0, 1.0}, {366.0, 0.0}});

  TestFrame season_frame;
  season_frame.declare("curve", "day", "kg", Attribute::Const, "curve");
  season_frame.set_check("curve", season_check);
  season_frame.set("curve", balanced);
  EXPECT_TRUE(frame_is_valid(metalib, season_frame));

  TestFrame fixed_point;
  fixed_point.declare("curve", "day", "kg", Attribute::Const, "curve");
  fixed_point.set_check("curve", fixed_point_check);
  fixed_point.set("curve", balanced);
  EXPECT_TRUE(frame_is_valid(metalib, fixed_point));

  TestFrame invalid_end;
  invalid_end.declare("curve", "day", "kg", Attribute::Const, "curve");
  invalid_end.set_check("curve", end_value_check);
  invalid_end.set("curve", make_plf({{1.0, 0.0}, {100.0, 1.0}, {366.0, 0.5}}));
  EXPECT_FALSE(frame_is_valid(metalib, invalid_end));
  EXPECT_NE(frame_errors(metalib, invalid_end).find("End value"), std::string::npos);
}
