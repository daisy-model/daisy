#include <vector>

#include <gtest/gtest.h>

#include "object_model/treelog_text.h"
#include "util/assertion.h"

TEST(AssertionTest, MessageWarningErrorAndDebugRouteToRegisteredLog) {
  TreelogString log;

  {
    Assertion::Register register_log(log);
    Assertion::message("plain");
    Assertion::warning("warn");
    Assertion::error("err");
    Assertion::debug("debug");
  }

  const std::string text = log.str();
  EXPECT_NE(text.find("plain\n"), std::string::npos);
  EXPECT_NE(text.find("warn (warning)\n"), std::string::npos);
  EXPECT_NE(text.find("err (error)\n"), std::string::npos);
  EXPECT_EQ(text.find("debug"), std::string::npos);
}

TEST(AssertionTest, FailureAndPanicThrowWhenLogIsRegistered) {
  TreelogString log;

  {
    Assertion::Register register_log(log);
    EXPECT_ANY_THROW(Assertion::failure("file.C", 10, "fun", "x > 0"));
    EXPECT_ANY_THROW(Assertion::panic("file.C", 12, "fun", "panic"));
  }

  EXPECT_FALSE(log.str().empty());
}

TEST(AssertionTest, BugAndWarningProduceDiagnosticsWithoutThrowing) {
  TreelogString log;

  {
    Assertion::Register register_log(log);
    Assertion::bug("file.C", 20, "fun", "bug");
    Assertion::warning("file.C", 21, "fun", "warn");
  }

  const std::string text = log.str();
  EXPECT_NE(text.find("bug"), std::string::npos);
  EXPECT_NE(text.find("warn"), std::string::npos);
}

TEST(AssertionTest, NonNegativeAndBalanceThrowOnInvalidInput) {
  TreelogString log;

  {
    Assertion::Register register_log(log);
    const std::vector<double> values{1.0, -1.0};
    EXPECT_ANY_THROW(Assertion::non_negative("file.C", 30, "fun", values));
    EXPECT_ANY_THROW(Assertion::balance("file.C", 31, "fun", 1.0, 3.0, 1.0));
  }

  EXPECT_FALSE(log.str().empty());
}

TEST(AssertionTest, ApproximateOnlyLogsWhenValuesDifferMaterially) {
  TreelogString log;

  {
    Assertion::Register register_log(log);
    Assertion::approximate("file.C", 40, "fun", 1.0, 1.0);
    EXPECT_TRUE(log.str().empty());
    Assertion::approximate("file.C", 41, "fun", 1.0, 2.0);
  }

  EXPECT_FALSE(log.str().empty());
}
