#include <cmath>
#include <memory>
#include <set>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "object_model/block_model.h"
#include "object_model/block_top.h"
#include "object_model/frame_model.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/treelog.h"
#include "object_model/unit_model.h"
#include "object_model/units.h"

namespace {

void load_test_frame(Frame& frame) {
  Units::load_syntax(frame);
}

std::set<symbol> library_entries(const Library& library) {
  std::vector<symbol> entries;
  library.entries(entries);
  return std::set<symbol>(entries.begin(), entries.end());
}

std::unique_ptr<FrameModel> clone_model(const Library& library,
                                        const symbol name) {
  return std::unique_ptr<FrameModel>(&library.model(name).clone());
}

void register_test_models() {
  register_unit_models();
}

}  // namespace

TEST(UnitModelRegistrationTest, UnitLibraryContainsExpectedModels) {
  register_test_models();
  Metalib metalib(load_test_frame);

  ASSERT_TRUE(metalib.exist(MUnit::component));
  const Library& library = metalib.library(MUnit::component);
  const std::set<symbol> entries = library_entries(library);

  EXPECT_TRUE(entries.count("SI"));
  EXPECT_TRUE(entries.count("SIfactor"));
  EXPECT_TRUE(entries.count("pF"));
  EXPECT_TRUE(entries.count("base"));
  EXPECT_TRUE(entries.count("factor"));
  EXPECT_TRUE(entries.count("offset"));
  EXPECT_TRUE(entries.count("cm"));
  EXPECT_TRUE(entries.count("rad"));
  EXPECT_TRUE(entries.count("dg"));
  EXPECT_TRUE(entries.count("dg C"));
}

TEST(UnitModelRegistrationTest, UnitCoreModelsHaveExpectedInheritance) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(MUnit::component);

  EXPECT_TRUE(library.check("SI"));
  EXPECT_TRUE(library.check("SIfactor"));
  EXPECT_TRUE(library.check("pF"));
  EXPECT_TRUE(library.check("base"));
  EXPECT_TRUE(library.check("factor"));
  EXPECT_TRUE(library.check("offset"));

  EXPECT_TRUE(library.is_derived_from("SI", "component"));
  EXPECT_TRUE(library.is_derived_from("SIfactor", "SI"));
  EXPECT_TRUE(library.is_derived_from("pF", "component"));
  EXPECT_TRUE(library.is_derived_from("base", "component"));
  EXPECT_TRUE(library.is_derived_from("factor", "component"));
  EXPECT_TRUE(library.is_derived_from("offset", "component"));

  EXPECT_EQ(library.base_model("SI"), symbol("component"));
  EXPECT_EQ(library.base_model("SIfactor"), symbol("component"));
  EXPECT_EQ(library.base_model("pF"), symbol("component"));
  EXPECT_EQ(library.base_model("base"), symbol("component"));
  EXPECT_EQ(library.base_model("factor"), symbol("component"));
  EXPECT_EQ(library.base_model("offset"), symbol("component"));
}

TEST(UnitModelRegistrationTest, UnitComponentSymbolIsStable) {
  EXPECT_EQ(symbol(MUnit::component), symbol("unit"));
}

TEST(UnitModelExposureTest, ConcreteUnitClassesArePublicTypes) {
  EXPECT_TRUE((std::is_base_of<Unit, MUnit>::value));
  EXPECT_TRUE((std::is_base_of<MUnit, UnitSI>::value));
  EXPECT_TRUE((std::is_base_of<UnitSI, UnitSIFactor>::value));
  EXPECT_TRUE((std::is_base_of<MUnit, UnitpF>::value));
  EXPECT_TRUE((std::is_base_of<MUnit, UnitBase>::value));
  EXPECT_TRUE((std::is_base_of<MUnit, UnitFactor>::value));
  EXPECT_TRUE((std::is_base_of<MUnit, UnitOffset>::value));
  EXPECT_TRUE((std::is_abstract<MUnit>::value));
  EXPECT_TRUE((std::is_abstract<UnitSI>::value));
  EXPECT_TRUE((std::is_constructible<UnitSIFactor, symbol, symbol, double>::value));
  EXPECT_TRUE((std::is_constructible<UnitpF>::value));
  EXPECT_TRUE((std::is_constructible<UnitBase, symbol>::value));
  EXPECT_TRUE((std::is_constructible<UnitFactor, symbol, symbol, double>::value));
  EXPECT_TRUE((std::is_constructible<UnitOffset, symbol, symbol, double, double>::value));
  EXPECT_TRUE((std::is_constructible<UnitSIFactor, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<UnitpF, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<UnitBase, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<UnitFactor, const BlockModel&>::value));
  EXPECT_TRUE((std::is_constructible<UnitOffset, const BlockModel&>::value));
}

TEST(UnitModelExposureTest, DirectConstructorsExposeExpectedConversions) {
  UnitSIFactor centimeter("cm", "m", 0.01);
  UnitpF pf;
  UnitBase radian("rad");
  UnitFactor degree("dg", "rad", M_PI / 180.0);
  UnitOffset celcius("dg C", "K", 1.0, 273.15);

  EXPECT_EQ(centimeter.native_name(), symbol("cm"));
  EXPECT_EQ(centimeter.base_name(), symbol("m"));
  EXPECT_DOUBLE_EQ(centimeter.to_base(2.0), 0.02);
  EXPECT_DOUBLE_EQ(centimeter.to_native(0.02), 2.0);

  EXPECT_EQ(pf.native_name(), symbol("pF"));
  EXPECT_EQ(pf.base_name(), Unit::pressure());
  EXPECT_DOUBLE_EQ(pf.to_base(2.0), -10000.0);
  EXPECT_DOUBLE_EQ(pf.to_native(-10000.0), 2.0);

  EXPECT_EQ(radian.base_name(), symbol("rad"));
  EXPECT_DOUBLE_EQ(radian.to_base(3.0), 3.0);

  EXPECT_EQ(degree.base_name(), symbol("rad"));
  EXPECT_NEAR(degree.to_base(180.0), M_PI, 1e-12);

  EXPECT_EQ(celcius.base_name(), symbol("K"));
  EXPECT_NEAR(celcius.to_base(0.0), 273.15, 1e-12);
  EXPECT_NEAR(celcius.to_native(273.15), 0.0, 1e-12);
}

TEST(UnitModelExposureTest, UnitModelsCanBeInstantiatedDirectlyFromBlockModel) {
  register_test_models();
  Metalib metalib(load_test_frame);
  const Library& library = metalib.library(MUnit::component);
  BlockTop context(metalib, Treelog::null(), metalib);

  std::unique_ptr<FrameModel> si_factor_frame = clone_model(library, "SIfactor");
  si_factor_frame->set("length", 1);
  si_factor_frame->set("factor", 0.01);
  BlockModel si_factor_block(context, *si_factor_frame, "SIfactor");
  UnitSIFactor si_factor(si_factor_block);
  EXPECT_EQ(si_factor.native_name(), symbol("SIfactor"));
  EXPECT_EQ(si_factor.base_name(), symbol("m"));
  EXPECT_DOUBLE_EQ(si_factor.to_base(2.0), 0.02);

  std::unique_ptr<FrameModel> pf_frame = clone_model(library, "pF");
  BlockModel pf_block(context, *pf_frame, "pF");
  UnitpF pf(pf_block);
  EXPECT_EQ(pf.native_name(), symbol("pF"));
  EXPECT_EQ(pf.base_name(), Unit::pressure());
  EXPECT_DOUBLE_EQ(pf.to_native(-10000.0), 2.0);

  std::unique_ptr<FrameModel> base_frame = clone_model(library, "base");
  BlockModel base_block(context, *base_frame, "base");
  UnitBase base(base_block);
  EXPECT_EQ(base.native_name(), symbol("base"));
  EXPECT_EQ(base.base_name(), symbol("base"));

  std::unique_ptr<FrameModel> factor_frame = clone_model(library, "factor");
  factor_frame->set("base", "rad");
  factor_frame->set("factor", M_PI / 180.0);
  BlockModel factor_block(context, *factor_frame, "factor");
  UnitFactor factor(factor_block);
  EXPECT_EQ(factor.native_name(), symbol("factor"));
  EXPECT_EQ(factor.base_name(), symbol("rad"));
  EXPECT_NEAR(factor.to_base(180.0), M_PI, 1e-12);

  std::unique_ptr<FrameModel> offset_frame = clone_model(library, "offset");
  offset_frame->set("base", "K");
  offset_frame->set("factor", 1.0);
  offset_frame->set("offset", 273.15);
  BlockModel offset_block(context, *offset_frame, "offset");
  UnitOffset offset(offset_block);
  EXPECT_EQ(offset.native_name(), symbol("offset"));
  EXPECT_EQ(offset.base_name(), symbol("K"));
  EXPECT_NEAR(offset.to_native(273.15), 0.0, 1e-12);
}
