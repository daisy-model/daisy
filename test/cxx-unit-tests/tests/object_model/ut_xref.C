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

TEST(XRefTest, SubmodelUsersWithDifferentPathsStayDistinct) {
  std::set<XRef::SubmodelUser> users;
  users.insert(XRef::SubmodelUser("AOM",
                                  std::vector<symbol>{"root_am"}));
  users.insert(XRef::SubmodelUser("AOM",
                                  std::vector<symbol>{"litter_am"}));

  ASSERT_EQ(users.size(), 2U);
}
