#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/check.h"
#include "object_model/frame.h"
#include "object_model/metalib.h"
#include "object_model/plf.h"
#include "object_model/units.h"

namespace {

class TestFrame : public Frame {
public:
  explicit TestFrame(const Frame* parent = nullptr) : parent_(parent) {}
  TestFrame(const TestFrame& other) : Frame(other), parent_(other.parent_) {}

  const Frame* parent() const override { return parent_; }

  void replace_parent(const Frame* new_parent) const override { parent_ = new_parent; }

  Frame& clone() const override { return *new TestFrame(*this); }

private:
  mutable const Frame* parent_;
};

PLF make_plf() {
  PLF plf;
  plf.add(1.0, 2.0);
  plf.add(3.0, 4.0);
  return plf;
}

}  // namespace

TEST(FrameTest, DeclarationsExposeEntriesTypeMetadataAndCitations) {
  TestFrame frame;

  frame.declare_number_cited("mass", "kg", Check::positive(), Attribute::OptionalConst,
                             Attribute::Variable, "Mass description",
                             std::vector<symbol>{symbol("ref-1"), symbol("ref-2")});
  frame.declare_text("notes", Attribute::Const, "Notes");
  frame.declare_fraction("fraction", Attribute::State, "Fraction");

  std::set<symbol> entries;
  frame.entries(entries);

  EXPECT_EQ(entries.size(), 3U);
  EXPECT_TRUE(entries.find("mass") != entries.end());
  EXPECT_TRUE(entries.find("notes") != entries.end());
  EXPECT_TRUE(entries.find("fraction") != entries.end());

  EXPECT_EQ(frame.lookup("mass"), Attribute::Number);
  EXPECT_EQ(frame.type_size("mass"), Attribute::Variable);
  EXPECT_TRUE(frame.is_optional("mass"));
  EXPECT_TRUE(frame.is_const("mass"));
  EXPECT_EQ(frame.dimension("mass"), symbol("kg"));
  EXPECT_EQ(frame.description("mass"), symbol("Mass description"));
  ASSERT_EQ(frame.type_cite("mass").size(), 2U);
  EXPECT_EQ(frame.type_cite("mass")[0], symbol("ref-1"));
  EXPECT_EQ(frame.type_cite("mass")[1], symbol("ref-2"));

  EXPECT_EQ(frame.lookup("notes"), Attribute::String);
  EXPECT_TRUE(frame.is_text("notes"));
  EXPECT_EQ(frame.description("notes"), symbol("Notes"));

  EXPECT_EQ(frame.lookup("fraction"), Attribute::Number);
  EXPECT_TRUE(frame.is_state("fraction"));
  EXPECT_EQ(frame.dimension("fraction"), Attribute::Fraction());
}

TEST(FrameTest, ReferencesAndValueMetadataRoundTrip) {
  TestFrame frame;

  frame.declare_string("target", Attribute::Const, "Target");
  frame.declare("mass", "kg", Attribute::Const, "Mass");
  frame.declare("curve", "day", "kg", Attribute::Const, "Curve");

  frame.set_reference("target", "soil.water");
  frame.set_described("mass", 2.5, "Measured mass");
  frame.set_cited("curve", make_plf(), "Calibration curve",
                  std::vector<symbol>{symbol("paper-a"), symbol("paper-b")});

  EXPECT_TRUE(frame.is_reference("target"));
  EXPECT_EQ(frame.get_reference("target"), symbol("soil.water"));
  EXPECT_FALSE(frame.check("target"));

  EXPECT_TRUE(frame.check("mass"));
  EXPECT_DOUBLE_EQ(frame.number("mass"), 2.5);
  EXPECT_EQ(frame.value_description("mass"), symbol("Measured mass"));
  EXPECT_TRUE(frame.value_cite("mass").empty());

  EXPECT_TRUE(frame.check("curve"));
  EXPECT_EQ(frame.value_description("curve"), symbol("Calibration curve"));
  ASSERT_EQ(frame.value_cite("curve").size(), 2U);
  EXPECT_EQ(frame.value_cite("curve")[0], symbol("paper-a"));
  EXPECT_EQ(frame.value_cite("curve")[1], symbol("paper-b"));
}

TEST(FrameTest, OrderingAndInheritedMetadataFollowParentUntilOverridden) {
  TestFrame parent;
  parent.declare_string("first", Attribute::Const, "First");
  parent.declare_integer("second", Attribute::Const, "Second");
  parent.declare_boolean("trace", Attribute::LogOnly, "Trace");
  parent.order("first", "second");

  TestFrame child(&parent);
  child.declare_string("third", Attribute::Const, "Third");

  EXPECT_TRUE(parent.ordered());
  EXPECT_TRUE(parent.total_order());
  EXPECT_TRUE(child.ordered());
  EXPECT_FALSE(child.total_order());
  EXPECT_EQ(child.order_index("first"), 0);
  EXPECT_EQ(child.order_index("second"), 1);
  EXPECT_EQ(child.lookup("first"), Attribute::String);
  EXPECT_EQ(child.description("first"), symbol("First"));

  child.order("third", "first", "second");
  EXPECT_TRUE(child.total_order());
  EXPECT_EQ(child.order_index("third"), 0);
}

TEST(FrameTest, EmptySequencesOverwriteSubsetAndResetBehaveConsistently) {
  Metalib metalib(Units::load_syntax);

  TestFrame source;
  source.declare_string("names", Attribute::Const, Attribute::Variable, "Names");
  source.declare_integer("counts", Attribute::Const, Attribute::Variable, "Counts");
  source.declare("mass", "kg", Attribute::Const, "Mass");
  source.order("mass", "names", "counts");
  source.set_strings("names", "a", "b");
  source.set("counts", std::vector<int>{1, 2});
  source.set_cited("mass", 3.5, "Copied mass", symbol("source-ref"));

  TestFrame copied;
  copied.declare_string("names", Attribute::Const, Attribute::Variable, "Names");
  copied.declare_integer("counts", Attribute::Const, Attribute::Variable, "Counts");
  copied.declare("mass", "kg", Attribute::Const, "Mass");
  copied.set_strings("names", "x");
  copied.set("counts", std::vector<int>{9});
  copied.set("mass", 1.0);
  copied.overwrite_values(source);

  ASSERT_EQ(copied.name_sequence("names").size(), 2U);
  EXPECT_EQ(copied.name_sequence("names")[0], symbol("a"));
  EXPECT_EQ(copied.name_sequence("names")[1], symbol("b"));
  ASSERT_EQ(copied.integer_sequence("counts").size(), 2U);
  EXPECT_EQ(copied.integer_sequence("counts")[0], 1);
  EXPECT_EQ(copied.integer_sequence("counts")[1], 2);
  EXPECT_DOUBLE_EQ(copied.number("mass"), 3.5);
  EXPECT_EQ(copied.value_description("mass"), symbol("Copied mass"));
  ASSERT_EQ(copied.value_cite("mass").size(), 1U);
  EXPECT_EQ(copied.value_cite("mass")[0], symbol("source-ref"));

  TestFrame partial;
  partial.declare_string("names", Attribute::Const, Attribute::Variable, "Names");
  partial.declare_integer("counts", Attribute::Const, Attribute::Variable, "Counts");
  partial.declare("mass", "kg", Attribute::Const, "Mass");
  partial.set("mass", 3.5);

  EXPECT_TRUE(partial.subset_elements(metalib, copied));
  EXPECT_FALSE(copied.subset_elements(metalib, partial));

  copied.set_empty("names");
  copied.set_empty("counts");
  EXPECT_EQ(copied.value_size("names"), 0);
  EXPECT_EQ(copied.value_size("counts"), 0);

  source.reset();
  std::set<symbol> entries;
  source.entries(entries);
  EXPECT_TRUE(entries.empty());
  EXPECT_FALSE(source.ordered());
  EXPECT_EQ(source.lookup("mass"), Attribute::Error);
}
