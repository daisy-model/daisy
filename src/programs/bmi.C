// daisy_bmi.C -- BMI 2.0 wrapper implementation for Daisy
//
// This file is part of Daisy.

#include "programs/bmi.h"
#include "daisy/daisy.h"
#include <stdexcept>
#include <limits>
#include <cmath>

// ===== STATIC VARIABLE LISTS =====

Daisy& BMI::daisy () { return ctrl_.daisy_ref (); }

const std::vector<std::string> BMI::INPUT_VARS = {
  "groundwater__depth",       // [cm] below surface
  "irrigation__rate",         // [mm/day]
};

const std::vector<std::string> BMI::OUTPUT_VARS = {
  "soil_water__recharge_rate",                // [mm/day]
  "land_surface__evapotranspiration_rate",    // [mm/day]
  "soil_water__transpiration_rate",           // [mm/day]
  "soil_water__evaporation_rate",             // [mm/day]
  "soil_water__drainage_rate",                // [mm/day]
  "soil_water__runoff_rate",                  // [mm/day]
  "groundwater__depth",                       // [cm]
  "vegetation__leaf_area_index",              // [-]
  "vegetation__root_depth",                   // [cm]
  "vegetation__aboveground_biomass",          // [g/m2]
  "soil_water__content",                      // [m3/m3] top layer
  "column__area",                             // [cm2] scalar
  "soil_layer__top_depth",                    // [cm] array, N layers
  "soil_layer__bottom_depth",                 // [cm] array, N layers
  "soil_water__pressure_head",                // [cm] array, N layers
  "soil_water__theta",                        // [-] array, N layers
  "soil__theta_sat",                          // [-] array, N layers (static soil param)
};

// ===== CONSTRUCTOR / DESTRUCTOR =====

BMI::BMI()
  : start_time_days_(0.0)
  , current_time_days_(0.0)
  , dt_days_(1.0 / 24.0)   // default: hourly timestep
{
}

BMI::~BMI()
{
  // DaisyBMI destructor handles cleanup
}

// ===== LIFECYCLE =====

void BMI::initialize(const std::string& config_file)
{
  try
  {
    ctrl_.initialize(config_file);
  }
  catch (const std::exception& e)
  {
    throw std::runtime_error(
      std::string("BMI::initialize failed for config: ") + config_file
      + "\n  Reason: " + e.what());
  }
  catch (const int code)
  {
    throw std::runtime_error(
      std::string("BMI::initialize failed for config: ") + config_file
      + "\n  Reason: Daisy reported an error (check daisy.log for details). Exit code: "
      + std::to_string(code));
  }
  catch (const char* msg)
  {
    throw std::runtime_error(
      std::string("BMI::initialize failed for config: ") + config_file
      + "\n  Reason: " + msg);
  }
  catch (...)
  {
    throw std::runtime_error(
      std::string("BMI::initialize failed for config: ") + config_file
      + "\n  Reason: unknown exception");
  }

  if (!ctrl_.is_initialized())
    throw std::runtime_error(
      std::string("BMI::initialize failed for config: ") + config_file);

  // Use Daisy's actual internal time as the reference point.
  // get_days_since_start() returns 0 right after init, but reading it through
  // the controller ensures we are in sync with Daisy's clock from the start.
  start_time_days_   = ctrl_.get_days_since_start(); // = 0.0 at init
  current_time_days_ = start_time_days_;

  dt_days_ = ctrl_.get_current_dt_days();
  if (dt_days_ <= 0.0)
    dt_days_ = 1.0 / 24.0; // fallback: hourly
}

void BMI::update()
{
  if (!ctrl_.tick())
    throw std::runtime_error("BMI::update (tick) failed");

  // Sync current time directly from Daisy's internal clock.
  current_time_days_ = ctrl_.get_days_since_start();

  // Keep dt in sync with Daisy's actual dt.
  double actual_dt = ctrl_.get_current_dt_days();
  if (actual_dt > 0.0)
    dt_days_ = actual_dt;
}

void BMI::update_until(double time)
{
  // Use half-timestep epsilon to avoid running one tick too many due to
  // floating-point accumulation (e.g. 24 * 1/24 is not exactly 1.0).
  const double eps = dt_days_ * 0.5;
  while (current_time_days_ < time - eps && ctrl_.is_running())
    update();

  // Snap to the exact requested time so callers always see a clean value
  // and drift does not accumulate across successive update_until calls.
  if (ctrl_.is_running())
    current_time_days_ = time;
}

void BMI::finalize()
{
  ctrl_.finalize();
}

// ===== INFORMATION =====

std::string BMI::get_component_name() const
{
  return "Daisy";
}

std::vector<std::string> BMI::get_input_var_names() const
{
  return INPUT_VARS;
}

std::vector<std::string> BMI::get_output_var_names() const
{
  return OUTPUT_VARS;
}

// ===== TIME =====

double BMI::get_start_time() const
{
  return start_time_days_;
}

double BMI::get_end_time() const
{
  return ctrl_.get_stop_days();
}

double BMI::get_current_time() const
{
  return current_time_days_;
}

double BMI::get_time_step() const
{
  return dt_days_;
}

std::string BMI::get_time_units() const
{
  return "d";
}

// ===== VARIABLE INFO =====

std::string BMI::get_var_type(const std::string& /*name*/) const
{
  return "double";
}

std::string BMI::get_var_units(const std::string& name) const
{
  if (name == "groundwater__depth")                    return "cm";
  if (name == "irrigation__rate")                      return "mm d-1";
  if (name == "soil_water__recharge_rate")             return "mm d-1";
  if (name == "land_surface__evapotranspiration_rate") return "mm d-1";
  if (name == "soil_water__transpiration_rate")        return "mm d-1";
  if (name == "soil_water__evaporation_rate")          return "mm d-1";
  if (name == "soil_water__drainage_rate")             return "mm d-1";
  if (name == "soil_water__runoff_rate")               return "mm d-1";
  if (name == "vegetation__leaf_area_index")           return "1";
  if (name == "vegetation__root_depth")                return "cm";
  if (name == "vegetation__aboveground_biomass")       return "g m-2";
  if (name == "soil_water__content")                   return "m3 m-3";
  if (name == "column__area")                          return "cm2";
  if (name == "soil_layer__top_depth")                 return "cm";
  if (name == "soil_layer__bottom_depth")              return "cm";
  if (name == "soil_water__pressure_head")             return "cm";
  if (name == "soil_water__theta")                     return "1";
  if (name == "soil__theta_sat")                       return "1";
  throw std::invalid_argument("Unknown variable: " + name);
}

int BMI::get_var_nbytes(const std::string& name) const {
  if (name == "soil_layer__top_depth" || name == "soil_layer__bottom_depth"
      || name == "soil_water__recharge_rate"
      || name == "soil_water__pressure_head"
      || name == "soil_water__theta"
      || name == "soil__theta_sat")
    return ctrl_.get_soil_layer_count() * sizeof(double);
  return sizeof(double);
}

int BMI::get_var_itemsize(const std::string& /*name*/) const
{
  return static_cast<int>(sizeof(double));
}

std::string BMI::get_var_location(const std::string& /*name*/) const
{
  return "none"; // scalars have no grid location
}

int BMI::get_var_grid(const std::string& /*name*/) const
{
  return 0; // all vars on the single-cell scalar grid
}

// ===== GRID INFO =====

int BMI::get_grid_rank(int /*grid*/) const  { return 0; }
int BMI::get_grid_size(int /*grid*/) const  { return 1; }
std::string BMI::get_grid_type(int /*grid*/) const { return "scalar"; }

// ===== GET / SET =====

double BMI::get_output_value(const std::string& name) const
{
  if (name == "soil_water__recharge_rate") {
    auto v = ctrl_.get_recharge_rate();
    return v.empty() ? 0.0 : v.back(); // scalar fallback = bottom-layer value
  }
  if (name == "land_surface__evapotranspiration_rate") return ctrl_.get_et_rate();
  if (name == "soil_water__transpiration_rate")        return ctrl_.get_transpiration_rate();
  if (name == "soil_water__evaporation_rate")          return ctrl_.get_evaporation_rate();
  if (name == "soil_water__drainage_rate")             return ctrl_.get_drainage_rate();
  if (name == "soil_water__runoff_rate")               return ctrl_.get_runoff_rate();
  if (name == "groundwater__depth")                    return ctrl_.get_groundwater_depth();
  if (name == "vegetation__leaf_area_index")           return ctrl_.get_leaf_area_index();
  if (name == "vegetation__root_depth")                return ctrl_.get_root_depth();
  if (name == "vegetation__aboveground_biomass")       return ctrl_.get_aboveground_biomass();
  if (name == "soil_water__content")
  {
    auto layers = ctrl_.get_soil_water_content();
    return layers.empty() ? std::numeric_limits<double>::quiet_NaN() : layers[0];
  }
  if (name == "column__area")                          return ctrl_.get_column_area();
  throw std::invalid_argument("Unknown output variable: " + name);
}

void BMI::get_value(const std::string& name, double* dest) const
{
  if (name == "soil_layer__top_depth") {
    auto v = ctrl_.get_layer_tops();
    std::copy(v.begin(), v.end(), dest);
    return;
  }
  if (name == "soil_layer__bottom_depth") {
    auto v = ctrl_.get_layer_bottoms();
    std::copy(v.begin(), v.end(), dest);
    return;
  }
  if (name == "soil_water__recharge_rate") {
    auto v = ctrl_.get_recharge_rate();
    std::copy(v.begin(), v.end(), dest);
    return;
  }
  if (name == "soil_water__pressure_head") {
    auto v = ctrl_.get_pressure_head_array();
    std::copy(v.begin(), v.end(), dest);
    return;
  }
  if (name == "soil_water__theta") {
    auto v = ctrl_.get_theta_array();
    std::copy(v.begin(), v.end(), dest);
    return;
  }
  if (name == "soil__theta_sat") {
    auto v = ctrl_.get_theta_sat_array();
    std::copy(v.begin(), v.end(), dest);
    return;
  }
  dest[0] = get_output_value(name);   // bestaand pad voor scalars
}

void BMI::set_value(const std::string& name, const double* src)
{
  if (name == "groundwater__depth")
  {
    ctrl_.set_groundwater_depth(src[0]);
    return;
  }
  if (name == "irrigation__rate")
  {
    // TODO: expose irrigation setter in DaisyBMI
    // ctrl_.set_irrigation_rate(src[0]);
    throw std::runtime_error("irrigation__rate setter not yet implemented in DaisyBMI");
  }
  throw std::invalid_argument("Unknown input variable: " + name);
}

