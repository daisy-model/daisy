#include <gtest/gtest.h>

#include <set>
#include <vector>

#include "object_model/xref.h"

TEST(XRefTest, ModelUsersWithDifferentPathsStayDistinct) {
  std::set<XRef::ModelUser> users;
  users.insert(XRef::ModelUser("vegetation", "afforestation",
                               std::vector<symbol>{"root_am"}));
  users.insert(XRef::ModelUser("vegetation", "afforestation",
                               std::vector<symbol>{"litter_am"}));

  ASSERT_EQ(users.size(), 2U);
}

TEST(XRefTest, ModelUsedOrdersByComponentThenModel) {
  const XRef::ModelUsed afforestation("vegetation", "afforestation");
  const XRef::ModelUsed residue("vegetation", "residue");
  const XRef::ModelUsed bioclimate("weather", "cloudiness");

  EXPECT_TRUE(afforestation < residue);
  EXPECT_TRUE(residue < bioclimate);
  EXPECT_FALSE(residue < afforestation);
}

TEST(XRefTest, ModelUsersDeduplicateIdenticalEntriesAndOrderByPath) {
  std::set<XRef::ModelUser> users;
  const XRef::ModelUser root("vegetation", "afforestation",
                             std::vector<symbol>{"root_am"});
  const XRef::ModelUser root_duplicate("vegetation", "afforestation",
                                       std::vector<symbol>{"root_am"});
  const XRef::ModelUser root_child("vegetation", "afforestation",
                                   std::vector<symbol>{"root_am", "leaf_am"});

  users.insert(root);
  users.insert(root_duplicate);
  users.insert(root_child);

  ASSERT_EQ(users.size(), 2U);
  EXPECT_TRUE(root < root_child);
  EXPECT_FALSE(root_child < root);
}

TEST(XRefTest, SubmodelUsersWithDifferentPathsStayDistinct) {
  std::set<XRef::SubmodelUser> users;
  users.insert(XRef::SubmodelUser("AOM",
                                  std::vector<symbol>{"root_am"}));
  users.insert(XRef::SubmodelUser("AOM",
                                  std::vector<symbol>{"litter_am"}));

  ASSERT_EQ(users.size(), 2U);
}

TEST(XRefTest, SubmodelUsersDefaultConstructAndOrderByPath) {
  const XRef::SubmodelUser root("AOM", std::vector<symbol>{"root_am"});
  const XRef::SubmodelUser root_child("AOM", std::vector<symbol>{"root_am", "leaf_am"});
  const XRef::SubmodelUser empty;

  EXPECT_TRUE(root < root_child);
  EXPECT_FALSE(root_child < root);
  EXPECT_EQ(empty.submodel, symbol());
  EXPECT_TRUE(empty.path.empty());
}

TEST(XRefTest, UsersStartEmpty) {
  const XRef::Users users;

  EXPECT_TRUE(users.models.empty());
  EXPECT_TRUE(users.submodels.empty());
}
