#include <memory>
#include <set>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>

#include "object_model/frame_model.h"
#include "object_model/function.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/parameter_types/boolean.h"
#include "object_model/units.h"

namespace {

void load_test_frame(Frame& frame) {
  Units::load_syntax(frame);
}

std::set<symbol> library_names(const Metalib& metalib) {
  std::vector<symbol> libraries;
  metalib.all(libraries);
  return std::set<symbol>(libraries.begin(), libraries.end());
}

boost::shared_ptr<const FrameModel> clone_model(const FrameModel& model) {
  return boost::shared_ptr<const FrameModel>(&model.clone());
}

}  // namespace

TEST(MetalibTest, AllListsCoreLibrariesAndExistMatchesLookup) {
  Metalib metalib(load_test_frame);
  const std::set<symbol> libraries = library_names(metalib);

  EXPECT_TRUE(libraries.count("boolean"));
  EXPECT_TRUE(libraries.count("function"));
  EXPECT_TRUE(libraries.count("unit"));
  EXPECT_EQ(libraries.count("does-not-exist"), 0U);

  EXPECT_TRUE(metalib.exist(Boolean::component));
  EXPECT_TRUE(metalib.exist(Function::component));
  EXPECT_FALSE(metalib.exist("does-not-exist"));
}

TEST(MetalibTest, SequenceAndParserFilesResetWithMetalibState) {
  Metalib metalib(load_test_frame);

  EXPECT_EQ(metalib.get_sequence(), 1);
  EXPECT_EQ(metalib.get_sequence(), 2);

  metalib.add_parser_file("alpha.dai");
  metalib.add_parser_file("beta.dai");
  ASSERT_EQ(metalib.parser_files().size(), 2U);
  EXPECT_EQ(metalib.parser_files()[0], symbol("alpha.dai"));
  EXPECT_EQ(metalib.parser_files()[1], symbol("beta.dai"));

  metalib.reset();

  EXPECT_TRUE(metalib.exist(Boolean::component));
  EXPECT_TRUE(metalib.exist(Function::component));
  EXPECT_TRUE(metalib.parser_files().empty());
  EXPECT_EQ(metalib.get_sequence(), 1);
}

TEST(MetalibTest, ParserInputsAreClonedFromProvidedFrames) {
  Metalib metalib(load_test_frame);
  std::vector<boost::shared_ptr<const FrameModel>> inputs;
  inputs.push_back(clone_model(metalib.library(Function::component).model("const")));
  inputs.push_back(clone_model(metalib.library(Boolean::component).model("and")));

  metalib.set_parser_inputs(inputs);

  ASSERT_EQ(metalib.parser_inputs().size(), inputs.size());
  EXPECT_NE(metalib.parser_inputs()[0], inputs[0].get());
  EXPECT_NE(metalib.parser_inputs()[1], inputs[1].get());

  const auto* first = dynamic_cast<const FrameModel*>(metalib.parser_inputs()[0]);
  const auto* second = dynamic_cast<const FrameModel*>(metalib.parser_inputs()[1]);
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(first->type_name(), symbol("const"));
  EXPECT_EQ(second->type_name(), symbol("and"));
}
