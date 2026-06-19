// daisy_bmi.C -- Python-controllable Daisy interface implementation
//
// This file is part of Daisy.

#define BUILD_DLL

#include "programs/daisy_bmi.h"
#include "object_model/toplevel.h"
#include "object_model/treelog.h"
#include "daisy/daisy.h"
#include "daisy/daisy_time.h"
#include "daisy/field.h"
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <boost/dll/runtime_symbol_info.hpp>

// ===== PRIVATE HELPERS =====

Daisy& DaisyPythonController::daisy() const
{
  return dynamic_cast<Daisy&>(toplevel_->program());
}

void DaisyPythonController::setup_logging()
{
  if (toplevel_) toplevel_->set_ui_none();
}

// Set DAISYHOME to the parent of the directory containing libcore.dll
// (i.e. the daisy source/install root), but only if DAISYHOME is not
// already set by the user. This ensures that when Daisy runs as a Python
// extension, it finds lib/ and sample/ relative to itself — not relative
// to python.exe.
static void ensure_daisy_home()
{
  if (getenv("DAISYHOME"))
    return; // user override takes priority

  try
  {
    // boost::dll::this_line_location() returns the path of the DLL that
    // contains the calling code — i.e. libcore.dll (or daisy_bmi.pyd).
    std::filesystem::path dll_path = boost::dll::this_line_location().native();
    // The DLLs sit in <daisy_root>/python/src/daisy/ (installed) or
    // <build_dir>/  (build tree). Walk up to find a directory that contains
    // both lib/ and sample/ — that is the Daisy home.
    std::filesystem::path candidate = dll_path.parent_path();
    for (int i = 0; i < 5; ++i)
    {
      if (std::filesystem::is_directory(candidate / "lib") &&
          std::filesystem::is_directory(candidate / "sample"))
      {
#ifdef _WIN32
        _putenv_s("DAISYHOME", candidate.string().c_str());
#else
        setenv("DAISYHOME", candidate.string().c_str(), 0);
#endif
        return;
      }
      candidate = candidate.parent_path();
    }
  }
  catch (...) {} // non-fatal: Daisy will fall back to its own detection
}

bool DaisyPythonController::load_config_file(const std::string& config_file)
{
  ensure_daisy_home();
  toplevel_ = std::make_unique<Toplevel>("daisy");
  toplevel_->set_ui_none();

  std::filesystem::path cfg_path = std::filesystem::absolute(config_file);
  std::filesystem::path cfg_dir  = cfg_path.parent_path();
  std::filesystem::path old_dir  = std::filesystem::current_path();
  try
  {
    std::filesystem::current_path(cfg_dir);
    toplevel_->parse_file(cfg_path.filename().string().c_str());
    std::filesystem::current_path(old_dir);
  }
  catch (...)
  {
    std::filesystem::current_path(old_dir);
    throw;
  }
  return true;
}

// ===== INITIALIZATION =====

bool DaisyPythonController::initialize(const std::string& config_file)
{
  // Let exceptions propagate — the caller (DaisyBMI) will wrap them
  // with a meaningful message. A silent 'return false' hides the root cause.
  load_config_file(config_file);
  start();
  return initialized;
}

// ===== LIFECYCLE =====

void DaisyPythonController::start()
{
  if (!toplevel_) return;
  toplevel_->initialize();   // moves state from is_uninitialized -> is_ready
  toplevel_->msg().open("Running");
  try
  {
    daisy().start();
    initialized = true;
    running = daisy().is_running();
  }
  catch (...)
  {
    toplevel_->msg().close();
    throw;
  }
}

bool DaisyPythonController::advance(double days)
{
  if (!initialized) return false;
  try
  {
    int steps = static_cast<int>(days * 24);
    for (int i = 0; i < steps && running; ++i)
    {
      daisy().tick(toplevel_->msg());
      running = daisy().is_running();
    }
    elapsed_days_ += days;
    return running;
  }
  catch (...) { return false; }
}

bool DaisyPythonController::tick()
{
  if (!initialized) return false;
  try
  {
    daisy().tick(toplevel_->msg());
    running = daisy().is_running();
    if (running) elapsed_days_ += 1.0 / 24.0;
    return running;
  }
  catch (...) { return false; }
}

void DaisyPythonController::stop()
{
  running = false;
}

bool DaisyPythonController::is_running() const
{
  return running;
}

bool DaisyPythonController::finalize()
{
  if (!initialized) return true;
  daisy().stop();
  daisy().summarize (toplevel_->msg());
  daisy().close_output ();   // flush + close all DLF file handles
  toplevel_->msg().close();
  initialized = false;
  running = false;
  return true;
}

// ===== TIME =====

double DaisyPythonController::get_days_since_start() const { return elapsed_days_; }

double DaisyPythonController::get_stop_days() const
{
  if (!initialized) return std::numeric_limits<double>::quiet_NaN();
  const double h = daisy().stop_duration_hours();
  return (h >= 0.0) ? h / 24.0 : std::numeric_limits<double>::quiet_NaN();
}

std::string DaisyPythonController::get_time_string() const
{
  if (!initialized) return "";
  const Time& t = daisy().time();
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:00",
                t.year(), t.month(), t.mday(), t.hour());
  return buf;
}

int DaisyPythonController::get_year()  const { return initialized ? daisy().time().year()  : 0; }
int DaisyPythonController::get_month() const { return initialized ? daisy().time().month() : 0; }
int DaisyPythonController::get_day()   const { return initialized ? daisy().time().mday()  : 0; }
int DaisyPythonController::get_hour()  const { return initialized ? daisy().time().hour()  : 0; }
double DaisyPythonController::get_current_dt_hours() const { return 1.0; }
double DaisyPythonController::get_current_dt_days()  const { return 1.0 / 24.0; }

// ===== GROUNDWATER =====

double DaisyPythonController::get_groundwater_depth() const
{
  return daisy().get_groundwater_table();
}

bool DaisyPythonController::set_groundwater_depth(double depth_cm)
{
  daisy().set_groundwater_table(depth_cm);
  return true;
}

auto DaisyPythonController::estimate_sy_perturbation(double dh_cm)
  -> std::tuple<double, std::vector<double>, std::vector<double>>
{
  return daisy().estimate_sy_perturbation(dh_cm);
}

double DaisyPythonController::get_pressure_head_at_depth(double) const
{
  return 0.0;  // stub
}

// ===== SOIL WATER =====

std::vector<double> DaisyPythonController::get_soil_water_content() const      { return {}; }
double DaisyPythonController::get_soil_water_at_depth(double) const            { return 0.0; }
double DaisyPythonController::get_matric_potential_at_depth(double) const      { return 0.0; }
double DaisyPythonController::get_cumulative_water_to_depth(double) const      { return 0.0; }
double DaisyPythonController::get_available_water() const                      { return 0.0; }

// ===== RECHARGE =====

std::vector<double> DaisyPythonController::get_recharge_rate() const
{
  std::vector<double> flux = daisy().get_flux_array();
  for (double& v : flux)
    v *= 10.0 * 24.0;  // cm/h -> mm/day
  return flux;
}

double DaisyPythonController::get_cumulative_recharge() const { return 0.0; }

std::vector<double> DaisyPythonController::get_pressure_head_array() const
{
  return daisy().get_h_array();  // [cm], nlayers
}

std::vector<double> DaisyPythonController::get_theta_array() const
{
  return daisy().get_theta_array();  // [-], nlayers
}

std::vector<double> DaisyPythonController::get_theta_sat_array() const
{
  return daisy().get_theta_sat_array();  // [-], nlayers, static soil param
}


// ===== WATER BALANCE STUBS =====

double DaisyPythonController::get_et_rate() const               { return 0.0; }
double DaisyPythonController::get_cumulative_et() const         { return 0.0; }
double DaisyPythonController::get_transpiration_rate() const    { return 0.0; }
double DaisyPythonController::get_evaporation_rate() const      { return 0.0; }
double DaisyPythonController::get_drainage_rate() const         { return 0.0; }
double DaisyPythonController::get_cumulative_drainage() const   { return 0.0; }
double DaisyPythonController::get_runoff_rate() const
{ return daisy().get_runoff_rate(); }  // [mm/day]
double DaisyPythonController::get_cumulative_runoff() const     { return 0.0; }

// ===== CROP STUBS =====

double DaisyPythonController::get_leaf_area_index() const       { return 0.0; }
double DaisyPythonController::get_root_depth() const            { return 0.0; }
double DaisyPythonController::get_aboveground_biomass() const   { return 0.0; }

// ===== SOIL STUBS =====

double DaisyPythonController::get_soil_temperature_at_depth(double) const { return 0.0; }
double DaisyPythonController::get_soil_layer_thickness(int) const         { return 0.0; }
double DaisyPythonController::get_soil_layer_top(int) const               { return 0.0; }
double DaisyPythonController::get_soil_layer_bottom(int) const            { return 0.0; }

// ===== WEATHER STUBS =====

double DaisyPythonController::get_rainfall_rate() const         { return 0.0; }
double DaisyPythonController::get_cumulative_rainfall() const   { return 0.0; }
double DaisyPythonController::get_reference_et() const          { return 0.0; }
double DaisyPythonController::get_air_temperature() const       { return 0.0; }
double DaisyPythonController::get_air_humidity() const          { return 0.0; }
double DaisyPythonController::get_wind_speed() const            { return 0.0; }

// ===== NITROGEN STUBS =====

double DaisyPythonController::get_mineral_nitrogen() const      { return 0.0; }
double DaisyPythonController::get_cumulative_n_leaching() const { return 0.0; }
double DaisyPythonController::get_cumulative_n_uptake() const   { return 0.0; }

// ===== DIAGNOSTICS STUBS =====

std::string DaisyPythonController::get_last_message() const     { return ""; }
bool        DaisyPythonController::has_errors() const           { return false; }
double      DaisyPythonController::get_simulation_time() const  { return 0.0; }

// ===== CONSTRUCTOR / DESTRUCTOR =====

DaisyPythonController::DaisyPythonController()
  : initialized(false), running(false)
{
}

DaisyPythonController::~DaisyPythonController()
{
  if (initialized)
  {
    try { finalize(); } catch (...) {}
  }
}

DaisyPythonController::DaisyPythonController(DaisyPythonController&& other) noexcept
  : toplevel_(std::move(other.toplevel_)),
    initialized(other.initialized),
    running(other.running),
    elapsed_days_(other.elapsed_days_)
{
  other.initialized   = false;
  other.running       = false;
  other.elapsed_days_ = 0.0;
}

DaisyPythonController& DaisyPythonController::operator=(DaisyPythonController&& other) noexcept
{
  if (this != &other)
  {
    if (initialized) try { finalize(); } catch (...) {}
    toplevel_           = std::move(other.toplevel_);
    initialized         = other.initialized;
    running             = other.running;
    elapsed_days_       = other.elapsed_days_;
    other.initialized   = false;
    other.running       = false;
    other.elapsed_days_ = 0.0;
  }
  return *this;
}

double DaisyPythonController::get_column_area() const
{ return daisy().get_column_area(); }

std::vector<double> DaisyPythonController::get_layer_tops() const
{ return daisy().get_layer_tops(); }

std::vector<double> DaisyPythonController::get_layer_bottoms() const
{ return daisy().get_layer_bottoms(); }

int DaisyPythonController::get_soil_layer_count() const
{ return static_cast<int>(daisy().get_layer_tops().size()); }
