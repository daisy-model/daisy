#include <random>
#include <gtest/gtest.h>

#include "object_model/treelog_text.h"
#include "daisy/upper_boundary/litter/retention.h"

struct RetentionTest : public testing::Test
{
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<> k_dis;
  std::uniform_real_distribution<> h_min_dis;
  std::uniform_real_distribution<> h_dis;
  std::uniform_real_distribution<> Theta_dis;
  std::uniform_real_distribution<> Theta_res_dis;
  std::uniform_real_distribution<> Theta_sat_dis;

  RetentionTest ()
    : rd(),
      gen(rd()),
      k_dis(0.001, 0.01),
      h_min_dis(-4e+06, -4e+03),
      h_dis(-400, 0),
      Theta_dis(0.35, 0.65),
      Theta_res_dis(0.0001, 0.35),
      Theta_sat_dis(0.65, 0.9999)
  { }
};

TEST_F (RetentionTest, PASTIS_h)
{
  double h_min = h_min_dis (gen);
  double Theta = Theta_dis (gen);
  double Theta_res = Theta_res_dis (gen);
  double Theta_sat = Theta_sat_dis (gen);
  RetentionPASTIS retention;
  TreelogString msg;
  retention.initialize(Theta_res, h_min, Theta_sat, msg);

  double h = - std::pow (-h_min, 1 - ((Theta - Theta_res) / (2.0/3.0 * Theta_sat - Theta_res)));
  EXPECT_GE (Theta, Theta_res);
  EXPECT_LE (Theta, Theta_sat);
  EXPECT_NEAR (h, retention.h (Theta), 1e-16);
}

TEST_F (RetentionTest, Exponential_ThetaHRelation)
{
  double k = k_dis(gen);
  double Theta_res = Theta_res_dis(gen);
  double h_min = h_min_dis(gen);
  double Theta_sat = Theta_sat_dis(gen);
  double h = h_dis(gen);
  RetentionExponential retention (k, Theta_res, h_min, Theta_sat);

  double Theta = retention.Theta (h);
  EXPECT_GE(Theta, Theta_res);
  EXPECT_LE(Theta, Theta_sat);
  EXPECT_DOUBLE_EQ (std::exp (k*h) * (Theta_sat - Theta_res) + Theta_res, Theta);
  EXPECT_NEAR (h, retention.h(Theta), 1e-10);
}
