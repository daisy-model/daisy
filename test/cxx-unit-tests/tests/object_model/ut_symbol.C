#include <gtest/gtest.h>

#include "object_model/symbol.h"

TEST(SymbolTest, IdenticalValuesHaveSameId) {
  int an_int = 10;
  const char *const a_char_p = "10";
  std::string a_string = a_char_p;

  symbol symbol0(an_int);
  symbol symbol1(a_char_p);
  symbol symbol2(a_string);
  ASSERT_EQ(symbol0.name(), symbol1.name());
  ASSERT_EQ(symbol1.name(), symbol2.name());
  ASSERT_EQ(symbol0, symbol1);
  ASSERT_EQ(symbol1, symbol2);
}

TEST(SymbolTest, DifferentValuesHaveSameDifferentId) {
  symbol symbol0 = "symbol0";
  symbol symbol1 = "symbol1";
  ASSERT_NE(symbol0, symbol1);
}


TEST(SymbolTest, SymbolDefinedFirstIsSmallest) {
  symbol symbol0 = "symbol0";
  symbol symbol1 = "symbol1";
  ASSERT_LT(symbol0, symbol1);
}

TEST(SymbolTest, AlphabeticalIsNameBased) {
  symbol symbol0 = "B_symbol0";
  symbol symbol1 = "A_symbol1";
  ASSERT_FALSE(symbol::alphabetical(symbol0, symbol1));
  ASSERT_TRUE(symbol::alphabetical(symbol1, symbol0));
}

TEST(SymbolTest, ConcateNames) {
  const char *const a_char_p = "abc";
  std::string a_string = "def";

  std::string char_p_string = a_char_p + a_string;
  std::string string_char_p = a_string + a_char_p;

  symbol symbol0(a_char_p);
  symbol symbol1(a_string);

  ASSERT_EQ(a_char_p + symbol1, char_p_string);
  ASSERT_EQ(symbol0 + a_string, char_p_string);
  ASSERT_EQ(symbol1 + a_char_p, string_char_p);
  ASSERT_EQ(a_string + symbol0, string_char_p);
  ASSERT_EQ(symbol0 + symbol1, char_p_string);
  ASSERT_EQ(symbol1 + symbol0, string_char_p);
}
