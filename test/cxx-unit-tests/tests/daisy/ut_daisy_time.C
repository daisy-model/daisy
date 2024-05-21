#include <gtest/gtest.h>

#include "object_model/symbol.h"
#include "daisy/daisy_time.h"

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
  Time time0{2024, 1, 1, 0, 0, 0, 0};
  time0.tick_microsecond(-1);
  EXPECT_EQ(time0.microsecond(), 999999);
  EXPECT_EQ(time0.second(), 59);
  EXPECT_EQ(time0.minute(), 59);
  EXPECT_EQ(time0.hour(), 23);
  EXPECT_EQ(time0.yday(), 365);
  EXPECT_EQ(time0.year(), 2023);
  
  Time time1{2021, 1, 10, 0, 0, 0, 0};
  time1.tick_day(-10);
  EXPECT_EQ(time1.yday(), 366);
  EXPECT_EQ(time1.year(), 2020);
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
  EXPECT_FALSE(Time::valid(0, 1, 1));
  EXPECT_FALSE(Time::valid(2024, 0, 1));
  EXPECT_FALSE(Time::valid(2024, 1, 0));
  EXPECT_TRUE(Time::valid(2024, 1, 1));
  
  EXPECT_FALSE(Time::valid(2024, 1, 1, 24));
  EXPECT_FALSE(Time::valid(2024, 1, 1, 23, 60));
  EXPECT_FALSE(Time::valid(2024, 1, 1, 23, 59, 1000000));
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

TEST_F(TimeTest, PrintTest) {
  Time nulltime = Time::null();
  EXPECT_EQ(nulltime.print(), "Time::null");
  
  Time time{1999,6,15,4,23,30,9999};
  EXPECT_EQ(time.print(), "1999-06-15T04:23:30.009999");
}


TEST_F(TimeTest, YearFractionTest) {
  Time time{2023,1,1,0};
  EXPECT_DOUBLE_EQ(time.year_fraction(), 1.0/365.0);
  time.tick_day(99);
  EXPECT_DOUBLE_EQ(time.year_fraction(), 100.0/365.0);
  time.tick_year(1);
  EXPECT_DOUBLE_EQ(time.year_fraction(), 100.0/366.0);
  time.tick_day(266);
  EXPECT_DOUBLE_EQ(time.year_fraction(), 1.0);
}

TEST_F(TimeTest, DayFractionTest) {
  Time time{2023,1,1,0};
  EXPECT_DOUBLE_EQ(time.day_fraction(), 0);
  time.tick_hour(12);
  EXPECT_DOUBLE_EQ(time.day_fraction(), 0.5);
  time.tick_hour(6);
  time.tick_minute(30);
  EXPECT_DOUBLE_EQ(time.day_fraction(), 18.5/24);
}

TEST_F(TimeTest, ComponentValueTest) {
  // 2023-01-02 is a Monday in week 1.
  Time time{2023,1,2,3,4,5,6};
  EXPECT_EQ(time.component_value(Time::Year), 2023);
  EXPECT_EQ(time.component_value(Time::Month), 1);
  EXPECT_EQ(time.component_value(Time::Week), 1);
  EXPECT_EQ(time.component_value(Time::Yday), 2);
  EXPECT_EQ(time.component_value(Time::Mday), 2);
  EXPECT_EQ(time.component_value(Time::Wday), 1);
  EXPECT_EQ(time.component_value(Time::Hour), 3);
  EXPECT_EQ(time.component_value(Time::Minute), 4);
  EXPECT_EQ(time.component_value(Time::Second), 5);
  EXPECT_EQ(time.component_value(Time::Microsecond), 6);
}

TEST_F(TimeTest, FindTimeComponentsTest) {
  std::vector<symbol> names{"year", "month", "week", "yday", "mday", "wday", "hour", "minute",
                            "second", "microsecond"};
  std::vector<Time::component_t> expected{
    Time::Year, Time::Month, Time::Week, Time::Yday, Time::Mday, Time::Wday, Time::Hour,
    Time::Minute, Time::Second, Time::Microsecond};

  std::vector<Time::component_t> actual = Time::find_time_components(names);
  ASSERT_EQ(actual.size(), expected.size());
  for (std::size_t i = 0; i < actual.size(); ++i) {
    ASSERT_EQ(actual[i], expected[i]);
  }
}

TEST_F(TimeTest, OperatorEqual) {
  Time now2 = Time::now();
  now2.tick_minute(1);
  ASSERT_TRUE(now == now);
  ASSERT_TRUE(now2 == now2);
  ASSERT_FALSE(now2 == now);
}

TEST_F(TimeTest, OperatorNotEqual) {
  Time now2 = Time::now();
  now2.tick_minute(1);
  ASSERT_FALSE(now != now);
  ASSERT_FALSE(now2 != now2);
  ASSERT_TRUE(now2 != now);
}

TEST_F(TimeTest, OperatorGreater) {
  Time now2 = Time::now();
  now2.tick_minute(1);
  ASSERT_FALSE(now > now);
  ASSERT_FALSE(now > now2);
  ASSERT_TRUE(now2 > now);
}

TEST_F(TimeTest, OperatorGreaterEqual) {
  Time now2 = Time::now();
  now2.tick_minute(1);
  ASSERT_TRUE(now >= now);
  ASSERT_FALSE(now >= now2);
  ASSERT_TRUE(now2 >= now);
}

TEST_F(TimeTest, OperatorLess) {
  Time now2 = Time::now();
  now2.tick_minute(1);
  ASSERT_FALSE(now < now);
  ASSERT_TRUE(now < now2);
  ASSERT_FALSE(now2 < now);
}

TEST_F(TimeTest, OperatorLessEqual) {
  Time now2 = Time::now();
  now2.tick_minute(1);
  ASSERT_TRUE(now <= now);
  ASSERT_TRUE(now <= now2);
  ASSERT_FALSE(now2 <= now);
}

TEST_F(TimeTest, BetweenTest) {
  Time time0{now.year() - 1, 1, 1, 0};
  Time time1{now.year() + 1, 1, 1, 0};
  ASSERT_TRUE(now.between(time0, time1));
  ASSERT_FALSE(time0.between(now, time1));
  ASSERT_FALSE(time1.between(time0, now));
}

TEST_F(TimeTest, WholeDaysBetweenTest) {
  Time time0{2024, 2, 28,  0};
  Time time1{2024, 3,  1, 10, 30};
  Time time2{2026, 4,  1, 10};
  ASSERT_EQ(Time::whole_days_between(time0, time1), 2);
  ASSERT_EQ(Time::whole_days_between(time0, time2), 366+365+31+1);
}


TEST_F(TimeTest, WholeHoursBetweenTest) {
  Time time0{2024, 2, 28, 0};
  Time time1{2024, 3,  1, 0};
  ASSERT_EQ(Time::whole_hours_between(time0, time1), 48);
}

TEST_F(TimeTest, FractionHoursBetweenTest) {
  Time time0{2024, 2, 28, 0};
  Time time1{2024, 3,  1, 0, 30};
  ASSERT_EQ(Time::fraction_hours_between(time0, time1), 48.5);
}
