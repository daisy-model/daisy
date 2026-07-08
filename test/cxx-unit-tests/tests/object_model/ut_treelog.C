#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>

#include "object_model/treelog.h"
#include "object_model/treelog_store.h"
#include "object_model/treelog_text.h"

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
