#include <gtest/gtest.h>

#include "symbol.h"
#include "daisy_time.h"

class TimeTest : public ::testing::Test {
protected:
  Time now;

  void SetUp() override {
    now = Time::now ();
  }
  
  void TearDown() override {
  }
};

TEST_F(TimeTest, NowTest) {
  const std::chrono::time_point cxx_now{std::chrono::system_clock::now()};
  const std::time_t c_now = std::chrono::system_clock::to_time_t(cxx_now);
  const std::tm local_now = *std::localtime(&c_now);
  
  EXPECT_EQ(now.year(), local_now.tm_year + 1900);
  EXPECT_EQ(now.month(), local_now.tm_mon + 1);
  //EXPECT_EQ(now.week(), local_now.tm_mday);
  EXPECT_EQ(now.yday(), local_now.tm_yday + 1); // Assume we start from 1
  EXPECT_EQ(now.mday(), local_now.tm_mday);
  EXPECT_EQ(now.wday(), 1 + (local_now.tm_wday - 1) % 7);
  EXPECT_EQ(now.hour(), local_now.tm_hour);
  EXPECT_EQ(now.minute(), local_now.tm_min);
}


TEST_F(TimeTest, TickTest) {
  int microsecond = now.microsecond();
  now.tick_microsecond(10);
  EXPECT_EQ(now.microsecond(), (microsecond + 10) % 1000000);

  int second = now.second();
  now.tick_second(10);
  EXPECT_EQ(now.second(), (second + 10) % 60);

  int minute = now.minute();
  now.tick_minute(10);
  EXPECT_EQ(now.minute(), (minute + 10) % 60);

  int hour = now.hour();
  now.tick_hour(10);
  EXPECT_EQ(now.hour(), (hour + 10) % 24);

  int wday = now.wday();
  int mday = now.mday();
  int yday = now.yday();
  int pre_tick_month_length = Time::month_length(now.year(), now.month());
  int pre_tick_year_length = 365 + (Time::leap(now.year()) ? 1 : 0);
  now.tick_day(10);
  EXPECT_EQ(now.wday(), 1 + (wday + 10 - 1) % 7);
  EXPECT_EQ(now.mday(), (mday + 10) % pre_tick_month_length);
  EXPECT_EQ(now.yday(), (yday + 10) % pre_tick_year_length);

  int year = now.year();
  now.tick_year(10);
  EXPECT_EQ(now.year(), year + 10);
}

TEST_F(TimeTest, TickOverflowTest) {
  Time time{2022, 1, 1, 0, 0, 0, 0};
  time.tick_microsecond(1e6 + 1e6 - 1);
  EXPECT_EQ(time.microsecond(), 1e6 - 1);
  EXPECT_EQ(time.second(), 1);

  time.tick_second(60+58);
  EXPECT_EQ(time.second(), 59);
  EXPECT_EQ(time.minute(), 1);

  time.tick_minute(60+58);
  EXPECT_EQ(time.minute(), 59);
  EXPECT_EQ(time.hour(), 1);

  time.tick_hour(24+22);
  EXPECT_EQ(time.hour(), 23);
  EXPECT_EQ(time.yday(), 2);

  time.tick_day(365+363);
  EXPECT_EQ(time.yday(), 365);
  EXPECT_EQ(time.year(), 2023);

  time.tick_microsecond(1);
  EXPECT_EQ(time.microsecond(), 0);
  EXPECT_EQ(time.second(), 0);
  EXPECT_EQ(time.minute(), 0);
  EXPECT_EQ(time.hour(), 0);
  EXPECT_EQ(time.yday(), 1);
  EXPECT_EQ(time.year(), 2024);
}

TEST_F(TimeTest, TickNegativeTest) {
  Time time{2024, 1, 1, 0, 0, 0, 0};
  time.tick_microsecond(-1);
  EXPECT_EQ(time.microsecond(), 999999);
  EXPECT_EQ(time.second(), 59);
  EXPECT_EQ(time.minute(), 59);
  EXPECT_EQ(time.hour(), 23);
  EXPECT_EQ(time.yday(), 365);
  EXPECT_EQ(time.year(), 2023);
}


TEST_F(TimeTest, LeapTest) {
  for (int year = 1800; year < 2200; ++year) {
    bool is_leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    ASSERT_EQ(Time::leap(year), is_leap);
  }
}

TEST_F(TimeTest, MonthLengthTest) {
  for (int year = 1800; year < 2200; ++year) {
    for (int month = 1; month <= 12; ++month) {
      int month_length = (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 ||
                          month == 10 || month == 12 ? 31 :
                          (month == 4 || month == 6 || month == 9 || month == 11 ? 30 :
                           28 + int(Time::leap(year))));
      ASSERT_EQ(Time::month_length(year, month), month_length);
    }
  }
}

TEST_F(TimeTest, WDayTest) {
  Time time0{2023, 12, 31, 0};
  EXPECT_EQ(time0.wday(), 7);
  
  Time time1{2024, 1, 1, 0};
  EXPECT_EQ(time1.wday(), 1);

  Time time2{2024, 1, 7, 0};
  EXPECT_EQ(time2.wday(), 7);
  
  Time time3{2024, 5, 16, 0};
  EXPECT_EQ(time3.wday(), 4);
}


TEST_F(TimeTest, YDay2MDayTest) {
  EXPECT_EQ(Time::yday2mday(now.year(), now.yday()), now.mday());  
}

TEST_F(TimeTest, YDay2WDayTest) {
  // yday2wday maps Monday=0 and Sunday=6
  // wday maps Monday=1 and Sunday=7
  EXPECT_EQ(Time::yday2wday(now.year(), now.yday()), now.wday() - 1);
  
  int year = 2024;  
  // 2024 is a leap year, so we check 2024-02-28, 2024-02-29 and 2024-03-01
  EXPECT_EQ(Time::yday2wday(year, 59), 2);
  EXPECT_EQ(Time::yday2wday(year, 60), 3);
  EXPECT_EQ(Time::yday2wday(year, 61), 4);

  // 2024-05-16 is day 137 and a Thursday (=3)
  EXPECT_EQ(Time::yday2wday(year, 137), 3);

  // 2024-01-01 is a Monday and yday2mday should map Monday=0 and Sunday=6, so we can just
  // subtract 1 module 7.
  for (int yday = 1; yday <= 14; ++yday) {
      ASSERT_EQ(Time::yday2wday(year, yday), (yday - 1) % 7);
  }
}

TEST_F(TimeTest, YDay2MonthTest) {
  EXPECT_EQ(Time::yday2month(now.year(), now.yday()), now.month());
  EXPECT_EQ(Time::yday2month(2024, 59), 2);
  EXPECT_EQ(Time::yday2month(2024, 60), 2);
  EXPECT_EQ(Time::yday2month(2024, 61), 3);

  EXPECT_EQ(Time::yday2month(2023, 365), 12);
  EXPECT_EQ(Time::yday2month(2023, 15), 1);
}

TEST_F(TimeTest, YDay2WeekTest) {
  EXPECT_EQ(Time::yday2week(now.year(), now.yday()), now.week());
  EXPECT_EQ(Time::yday2week(1978, 1), 52);
  EXPECT_EQ(Time::yday2week(1978, 2),  1);
}


TEST_F(TimeTest, MDay2YDayTest) {
  EXPECT_EQ(Time::mday2yday(now.year(), now.month(), now.mday()), now.yday());
  
  EXPECT_EQ(Time::mday2yday(2024, 1, 1), 1);
  // 2024 is a leap year, so we check if wday is correct for 2024-02-28, 2024-02-29 and 2024-03-01
  EXPECT_EQ(Time::mday2yday(2024, 2, 28), 59);
  EXPECT_EQ(Time::mday2yday(2024, 2, 29), 60);
  EXPECT_EQ(Time::mday2yday(2024, 3,  1), 61);

  EXPECT_EQ(Time::mday2yday(2024, 5, 16), 137);
}

TEST_F(TimeTest, ValidTest) {
  EXPECT_FALSE(Time::valid(2024, 0, 1));
  EXPECT_FALSE(Time::valid(2024, 1, 0));
  EXPECT_TRUE(Time::valid(2024, 1, 1));
}


TEST_F(TimeTest, MonthNameTest) {
  EXPECT_EQ(Time::month_name(1), symbol{"January"});
  EXPECT_EQ(Time::month_name(2), symbol{"February"});
  EXPECT_EQ(Time::month_name(3), symbol{"March"});
  EXPECT_EQ(Time::month_name(4), symbol{"April"});
  EXPECT_EQ(Time::month_name(5), symbol{"May"});
  EXPECT_EQ(Time::month_name(6), symbol{"June"});
  EXPECT_EQ(Time::month_name(7), symbol{"July"});
  EXPECT_EQ(Time::month_name(8), symbol{"August"});
  EXPECT_EQ(Time::month_name(9), symbol{"September"});
  EXPECT_EQ(Time::month_name(10), symbol{"October"});
  EXPECT_EQ(Time::month_name(11), symbol{"November"});
  EXPECT_EQ(Time::month_name(12), symbol{"December"});
}

TEST_F(TimeTest, MonthNumberTest) {
  EXPECT_EQ(Time::month_number(symbol{"January"}), 1);
  EXPECT_EQ(Time::month_number(symbol{"February"}), 2);
  EXPECT_EQ(Time::month_number(symbol{"March"}), 3);
  EXPECT_EQ(Time::month_number(symbol{"April"}), 4);
  EXPECT_EQ(Time::month_number(symbol{"May"}), 5);
  EXPECT_EQ(Time::month_number(symbol{"June"}), 6);
  EXPECT_EQ(Time::month_number(symbol{"July"}), 7);
  EXPECT_EQ(Time::month_number(symbol{"August"}), 8);
  EXPECT_EQ(Time::month_number(symbol{"September"}), 9);
  EXPECT_EQ(Time::month_number(symbol{"October"}), 10);
  EXPECT_EQ(Time::month_number(symbol{"November"}), 11);
  EXPECT_EQ(Time::month_number(symbol{"December"}), 12);
}


TEST_F(TimeTest, WDayNameTest) {
  EXPECT_EQ(Time::wday_name(1), symbol{"Monday"});
  EXPECT_EQ(Time::wday_name(2), symbol{"Tuesday"});
  EXPECT_EQ(Time::wday_name(3), symbol{"Wednesday"});
  EXPECT_EQ(Time::wday_name(4), symbol{"Thursday"});
  EXPECT_EQ(Time::wday_name(5), symbol{"Friday"});
  EXPECT_EQ(Time::wday_name(6), symbol{"Saturday"});
  EXPECT_EQ(Time::wday_name(7), symbol{"Sunday"});
}

TEST_F(TimeTest, WDayNumberTest) {
  EXPECT_EQ(Time::wday_number(symbol{"Monday"}), 1);
  EXPECT_EQ(Time::wday_number(symbol{"Tuesday"}), 2);
  EXPECT_EQ(Time::wday_number(symbol{"Wednesday"}), 3);
  EXPECT_EQ(Time::wday_number(symbol{"Thursday"}), 4);
  EXPECT_EQ(Time::wday_number(symbol{"Friday"}), 5);
  EXPECT_EQ(Time::wday_number(symbol{"Saturday"}), 6);
  EXPECT_EQ(Time::wday_number(symbol{"Sunday"}), 7);
}
