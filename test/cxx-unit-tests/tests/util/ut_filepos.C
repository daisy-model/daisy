#include <gtest/gtest.h>

#include "object_model/attribute.h"
#include "util/filepos.h"

TEST(FileposTest, ExposesConstructionArguments) {
  const Filepos pos("example.dai", 12, 34);

  EXPECT_EQ(pos.filename(), symbol("example.dai"));
  EXPECT_EQ(pos.line(), 12);
  EXPECT_EQ(pos.column(), 34);
}

TEST(FileposTest, NoneUsesSentinelValues) {
  const Filepos& none = Filepos::none();

  EXPECT_EQ(none.filename(), Attribute::None());
  EXPECT_EQ(none.line(), -42);
  EXPECT_EQ(none.column(), -42);
  EXPECT_EQ(&none, &Filepos::none());
}

TEST(FileposTest, EqualityAndInequalityCompareAllFields) {
  const Filepos original("same.dai", 2, 3);
  const Filepos same("same.dai", 2, 3);
  const Filepos different_line("same.dai", 4, 3);

  EXPECT_TRUE(original == same);
  EXPECT_FALSE(original != same);
  EXPECT_FALSE(original == different_line);
  EXPECT_TRUE(original != different_line);
}

TEST(FileposTest, OrderingOnlyAppliesWithinSameFile) {
  const Filepos first("same.dai", 1, 5);
  const Filepos second("same.dai", 2, 1);
  const Filepos third("same.dai", 2, 7);
  const Filepos other_file("other.dai", 0, 0);

  EXPECT_TRUE(first < second);
  EXPECT_TRUE(second < third);
  EXPECT_FALSE(third < second);
  EXPECT_FALSE(first < other_file);
  EXPECT_FALSE(other_file < first);
}
