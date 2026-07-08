#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "util/path.h"

namespace {

std::filesystem::path make_temp_dir(const std::string& prefix) {
  const auto base = std::filesystem::temp_directory_path();
  for (int attempt = 0; attempt < 100; ++attempt) {
    const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto candidate = base / (prefix + "-" + std::to_string(tick) + "-" + std::to_string(attempt));
    if (std::filesystem::create_directory(candidate)) {
      return candidate;
    }
  }
  throw std::runtime_error("failed to create temp directory");
}

std::string slurp(std::unique_ptr<std::istream> input) {
  std::ostringstream buffer;
  buffer << input->rdbuf();
  return buffer.str();
}

}  // namespace

TEST(PathTest, NodirReturnsLastPathComponent) {
#ifdef __unix
  EXPECT_EQ(Path::nodir("/tmp/example/file.dai"), symbol("file.dai"));
#else
  EXPECT_EQ(Path::nodir("C:\\tmp\\example\\file.dai"), symbol("file.dai"));
#endif
  EXPECT_EQ(Path::nodir("file.dai"), symbol("file.dai"));
}

TEST(PathTest, OpenFileSearchesInputDirectoryAndConfiguredPath) {
  const std::filesystem::path root = make_temp_dir("daisy-path-open");
  const std::filesystem::path extra = root / "extra";
  std::filesystem::create_directory(extra);

  {
    std::ofstream(root / "input.txt") << "from-input";
    std::ofstream(extra / "fallback.txt") << "from-extra";

    Path path;
    path.set_input_directory(root.string());
    path.set_path(std::vector<symbol>{".", extra.string()});

    EXPECT_EQ(slurp(path.open_file("input.txt")), "from-input");
    EXPECT_EQ(slurp(path.open_file("fallback.txt")), "from-extra");
  }

  std::filesystem::remove_all(root);
}

TEST(PathTest, SetDirectoryCreatesAndRestoresDirectoriesViaInDirectory) {
  const std::filesystem::path root = make_temp_dir("daisy-path-dir");
  const std::filesystem::path child = root / "child";

  Path path;
  const std::string original = path.get_output_directory().name();

  ASSERT_TRUE(path.set_directory(root.string()));
  EXPECT_EQ(path.get_output_directory(), symbol(root.string()));
  EXPECT_TRUE(std::filesystem::equivalent(std::filesystem::current_path(), root));

  {
    Path::InDirectory in_directory(path, child.string());
    EXPECT_TRUE(in_directory.check());
    EXPECT_EQ(path.get_output_directory(), symbol(child.string()));
    EXPECT_TRUE(std::filesystem::exists(child));
    EXPECT_TRUE(std::filesystem::equivalent(std::filesystem::current_path(), child));
  }

  EXPECT_EQ(path.get_output_directory(), symbol(root.string()));
  EXPECT_TRUE(std::filesystem::equivalent(std::filesystem::current_path(), root));

  ASSERT_TRUE(path.set_directory(original));
  std::filesystem::remove_all(root);
}

TEST(PathTest, StringPathSetterUsesSeparatorDelimitedEntries) {
  const std::filesystem::path root = make_temp_dir("daisy-path-list");
  const std::filesystem::path first = root / "first";
  const std::filesystem::path second = root / "second";
  std::filesystem::create_directory(first);
  std::filesystem::create_directory(second);
  std::ofstream(second / "value.txt") << "second";

  {
    Path path;
    path.set_input_directory(root.string());
#ifdef __unix
    path.set_path(first.string() + ":" + second.string());
#else
    path.set_path(first.string() + ";" + second.string());
#endif
    EXPECT_EQ(slurp(path.open_file("value.txt")), "second");
  }

  std::filesystem::remove_all(root);
}
