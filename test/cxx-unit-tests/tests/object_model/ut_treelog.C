#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "object_model/treelog.h"
#include "object_model/treelog_store.h"
#include "object_model/treelog_text.h"

namespace {

class RecordingTreelog : public Treelog {
public:
  std::vector<std::string> opened;
  std::vector<std::string> entries;
  std::vector<std::string> debugs;
  int closes = 0;
  int touches = 0;
  int flushes = 0;

  void open(const std::string& name) override { opened.push_back(name); }
  void close() override { closes++; }
  void debug(const std::string& text) override { debugs.push_back(text); }
  void entry(const std::string& text) override { entries.push_back(text); }
  void touch() override { touches++; }
  void flush() override { flushes++; }
};

}  // namespace

TEST(TreelogTest, OpenWritesNestedHeadersOncePerLevel) {
  TreelogString log;

  {
    Treelog::Open root(log, "root");
    log.entry("first");
    log.entry("second");
    {
      Treelog::Open child(log, "child");
      log.entry("nested");
    }
    log.entry("after");
  }

  EXPECT_EQ(log.str(), "* root\nfirst\nsecond\n** child\nnested\nafter\n");
}

TEST(TreelogTest, MessageWarningErrorAndBugUsePublicEntryFormatting) {
  TreelogString log;

  log.message("plain");
  log.warning("warn");
  log.error("err");
  log.bug("bug");

  EXPECT_EQ(log.str(), "plain\nwarn (warning)\nerr (error)\nbug (bug)\n");
}

TEST(TreelogTest, OpenFormatsIndexedSubmodelLabels) {
  TreelogString log;

  {
    Treelog::Open indexed(log, symbol("soil"), 1U, symbol("water"));
    log.entry("value");
  }

  EXPECT_EQ(log.str(), "* soil[1]: water\nvalue\n");
}

TEST(TreelogTest, StoreReplaysBufferedEntriesToLateClients) {
  TreelogStore store;
  EXPECT_TRUE(store.has_unhandled_events());

  store.open("root");
  store.entry("first");
  store.message("second");

  boost::shared_ptr<TreelogString> client(new TreelogString());
  store.add_client(client);

  EXPECT_FALSE(store.has_unhandled_events());
  EXPECT_EQ(client->str(), "* root\nfirst\nsecond\n");

  store.entry("third");
  EXPECT_EQ(client->str(), "* root\nfirst\nsecond\nthird\n");
}

TEST(TreelogTest, StoreCanBeClosedWithoutUnhandledEvents) {
  TreelogStore store;

  store.entry("first");
  EXPECT_TRUE(store.has_unhandled_events());

  store.no_more_clients();
  EXPECT_FALSE(store.has_unhandled_events());
}

TEST(TreelogTest, StorePropagateDebugRoutesAllMessagesThroughDebugHook) {
  TreelogStore store;
  RecordingTreelog recorder;

  store.open("root");
  store.debug("debug");
  store.entry("entry");
  store.warning("warning");
  store.touch();
  store.flush();
  store.close();

  store.propagate_debug(recorder);

  ASSERT_EQ(recorder.opened.size(), 1U);
  EXPECT_EQ(recorder.opened[0], "root");
  EXPECT_EQ(recorder.debugs.size(), 3U);
  EXPECT_EQ(recorder.debugs[0], "debug");
  EXPECT_EQ(recorder.debugs[1], "entry");
  EXPECT_EQ(recorder.debugs[2], "warning");
  EXPECT_EQ(recorder.entries.size(), 0U);
  EXPECT_EQ(recorder.touches, 1);
  EXPECT_EQ(recorder.flushes, 1);
  EXPECT_EQ(recorder.closes, 1);
}
