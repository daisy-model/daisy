// daisy_bmi.h -- Python-controllable Daisy interface
//
// This file is part of Daisy.
// 
// Daisy is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.

#ifndef DAISY_BMI_H
#define DAISY_BMI_H

#include <memory>
#include <string>
#include <tuple>
#include <vector>

// Forward declarations
class Toplevel;
class Daisy;

/**
 * @class DaisyController
 * @brief Python-friendly interface to control Daisy simulation externally
 * 
 * Allows Python code to:
 * - Initialize simulation from .dai config file
 * - Advance simulation per timestep
 * - Read simulation state (time, water, ET, recharge, etc.)
 * - Set boundary conditions (GW depth, rainfall, irrigation, etc.)
 * - Control simulation parameters
 */
class DaisyController
{
private:
  // Toplevel owns Metalib, parser, Treelog, and the Daisy program instance.
  std::unique_ptr<Toplevel> toplevel_;
  bool initialized;
  bool running;

  // Elapsed simulation time in days, incremented each tick().
  // Matches MODFLOW's approach: a simple counter, independent of calendar dates.
  double elapsed_days_ = 0.0;

  // Private helper — returns Daisy& from toplevel (after initialize)
  Daisy& daisy() const;

  // Private helper methods
  void setup_logging();
  bool load_config_file(const std::string& config_file);
  
public:
  // ===== INITIALIZATION =====
  
  /**
   * Initialize simulation from .dai configuration file
   * @param config_file Path to .dai configuration file
   * @return true if successful, false otherwise
   */
  bool initialize(const std::string& config_file);
  
  /**
   * Check if simulation is initialized
   * @return true if initialized
   */
  bool is_initialized() const { return initialized; }
  
  
  // ===== SIMULATION CONTROL =====
  
  /**
   * Advance simulation by specified number of days
   * @param days Number of days to advance
   * @return true if successful, false if error
   */
  bool advance(double days);
  
  /**
   * Advance simulation one internal timestep
   * @return true if successful, false if error
   */
  bool tick();
  
  /**
   * Start simulation
   */
  void start();
  
  /**
   * Stop simulation
   */
  void stop();
  
  /**
   * Check if simulation is still running
   * @return true if simulation is running
   */
  bool is_running() const;
  
  /**
   * Finalize simulation and cleanup
   * @return true if successful
   */
  bool finalize();
  
  
  // ===== TIME ACCESS =====
  
  /**
   * Get current simulation time as string
   * @return Time formatted as string (YYYY-MM-DD HH:MM)
   */
  std::string get_time_string() const;
  
  /**
   * Get current year
   * @return Year (e.g., 2020)
   */
  int get_year() const;
  
  /**
   * Get current month (1-12)
   * @return Month
   */
  int get_month() const;
  
  /**
   * Get current day of month (1-31)
   * @return Day
   */
  int get_day() const;
  
  /**
   * Get current hour (0-23)
   * @return Hour
   */
  int get_hour() const;
  
  /**
   * Get number of days simulated so far
   * @return Days from simulation start
   */
  double get_days_since_start() const;

  /**
   * Get the total duration of the simulation in days.
   * @return Days from simulation start to configured stop time,
   *         or NaN if the simulation has no fixed end date (open-ended).
   */
  double get_stop_days() const;

  /**
   * Get current timestep size in hours
   * @return Current dt in hours
   */
  double get_current_dt_hours() const;
  
  /**
   * Get current timestep size in days
   * @return Current dt in days
   */
  double get_current_dt_days() const;
  //====== DISCRETISATION INFO ======
  double              get_column_area   () const;
  std::vector<double> get_layer_tops    () const;
  std::vector<double> get_layer_bottoms () const;
  
  // ===== GROUNDWATER STATE =====
  
  /**
   * Get groundwater table depth
   * @return Groundwater depth in cm below surface (negative = above surface)
   */
  double get_groundwater_depth() const;
  
  /**
   * Set groundwater table depth (boundary condition)
   * @param depth_cm Depth in cm below surface (negative = above surface)
   * @return true if successful
   */
  bool set_groundwater_depth(double depth_cm);

  /**
   * Estimate specific yield via GW head perturbation + Richards re-solve.
   * @param dh_cm  Head perturbation in cm (default 1 cm)
   * @return Estimated Sy [-]
   */
  std::tuple<double, std::vector<double>, std::vector<double>>
    estimate_sy_perturbation(double dh_cm = 1.0);

  /**
   * Get pressure head at specific depth
   * @param z_cm Depth in cm (positive downward)
   * @return Pressure head in cm at that depth
   */
  double get_pressure_head_at_depth(double z_cm) const;
  
  
  // ===== SOIL WATER STATE =====
  
  /**
   * Get volumetric water content for all soil layers
   * @return Vector of theta values (m3/m3) for each layer, top to bottom
   */
  std::vector<double> get_soil_water_content() const;
  
  /**
   * Get volumetric water content at specific depth
   * @param z_cm Depth in cm (positive downward)
   * @return Theta (m3/m3)
   */
  double get_soil_water_at_depth(double z_cm) const;
  
  /**
   * Get water potential at specific depth
   * @param z_cm Depth in cm (positive downward)
   * @return Matric potential in cm
   */
  double get_matric_potential_at_depth(double z_cm) const;
  
  /**
   * Get cumulative water content from surface to depth
   * @param z_cm Depth in cm (positive downward)
   * @return Cumulative water content in mm (0 to depth)
   */
  double get_cumulative_water_to_depth(double z_cm) const;
  
  /**
   * Get total plant available water in profile
   * @return Available water in mm
   */
  double get_available_water() const;
  
  
  // ===== WATER BALANCE =====
  
  /**
   * Get current actual evapotranspiration
   * @return ET rate in mm/day
   */
  double get_et_rate() const;
  
  /**
   * Get cumulative evapotranspiration since simulation start
   * @return Cumulative ET in mm
   */
  double get_cumulative_et() const;
  
  /**
   * Get current actual transpiration from plant
   * @return Transpiration in mm/day
   */
  double get_transpiration_rate() const;
  
  /**
   * Get current actual evaporation from soil
   * @return Evaporation in mm/day
   */
  double get_evaporation_rate() const;
  
  /**
   * Get recharge/percolation to groundwater per soil layer
   * @return Flux at bottom edge of each layer [mm/day], nlayers long
   */
  std::vector<double> get_recharge_rate() const;

  /**
   * Get soil pressure head per layer
   * @return Pressure head [cm] per layer, nlayers long
   */
  std::vector<double> get_pressure_head_array() const;

  /**
   * Get volumetric water content per layer
   * @return Theta [-] per layer, nlayers long
   */
  std::vector<double> get_theta_array() const;

  /**
   * Get saturated volumetric water content per layer (static soil parameter)
   * @return Theta_sat [-] per layer, nlayers long
   */
  std::vector<double> get_theta_sat_array() const;

  /**
   * Get cumulative recharge since simulation start
   * @return Cumulative recharge in mm
   */
  double get_cumulative_recharge() const;
  
  /**
   * Get tile drainage flux
   * @return Drainage in mm/day
   */
  double get_drainage_rate() const;
  
  /**
   * Get cumulative tile drainage since simulation start
   * @return Cumulative drainage in mm
   */
  double get_cumulative_drainage() const;
  
  /**
   * Get surface runoff
   * @return Runoff in mm/day
   */
  double get_runoff_rate() const;
  
  /**
   * Get cumulative runoff since simulation start
   * @return Cumulative runoff in mm
   */
  double get_cumulative_runoff() const;
  
  
  // ===== CROP/VEGETATION STATE =====
  
  /**
   * Get leaf area index (LAI)
   * @return LAI (m2 leaf / m2 ground)
   */
  double get_leaf_area_index() const;
  
  /**
   * Get root depth
   * @return Root depth in cm
   */
  double get_root_depth() const;
  
  /**
   * Get aboveground biomass
   * @return Biomass in g/m2 dry matter
   */
  double get_aboveground_biomass() const;
  
  
  // ===== SOIL STATE =====
  
  /**
   * Get soil temperature at specific depth
   * @param z_cm Depth in cm (positive downward)
   * @return Temperature in Celsius
   */
  double get_soil_temperature_at_depth(double z_cm) const;
  
  /**
   * Get number of soil layers
   * @return Number of layers
   */
  int get_soil_layer_count() const;
  
  /**
   * Get depth of soil layer
   * @param layer Layer index (0-based, top to bottom)
   * @return Layer thickness in cm
   */
  double get_soil_layer_thickness(int layer) const;
  
  /**
   * Get soil layer top depth
   * @param layer Layer index
   * @return Depth to top of layer in cm
   */
  double get_soil_layer_top(int layer) const;
  
  /**
   * Get soil layer bottom depth
   * @param layer Layer index
   * @return Depth to bottom of layer in cm
   */
  double get_soil_layer_bottom(int layer) const;
  
  
  // ===== WEATHER INPUT =====
  
  /**
   * Get current rainfall rate
   * @return Rainfall in mm/day
   */
  double get_rainfall_rate() const;
  
  /**
   * Get cumulative rainfall since simulation start
   * @return Cumulative rainfall in mm
   */
  double get_cumulative_rainfall() const;
  
  /**
   * Get reference evapotranspiration (PET)
   * @return Reference ET in mm/day
   */
  double get_reference_et() const;
  
  /**
   * Get air temperature
   * @return Temperature in Celsius
   */
  double get_air_temperature() const;
  
  /**
   * Get air relative humidity
   * @return Relative humidity (0-100) in percent
   */
  double get_air_humidity() const;
  
  /**
   * Get wind speed
   * @return Wind speed in m/s
   */
  double get_wind_speed() const;
  
  
  // ===== NITROGEN STATE =====
  
  /**
   * Get mineral nitrogen in soil profile
   * @return Nitrogen in kg/ha
   */
  double get_mineral_nitrogen() const;
  
  /**
   * Get cumulative N leaching
   * @return N lost in mm
   */
  double get_cumulative_n_leaching() const;
  
  /**
   * Get cumulative N uptake by crop
   * @return N uptake in kg/ha
   */
  double get_cumulative_n_uptake() const;
  
  
  // ===== DIAGNOSTICS =====
  
  /**
   * Get last error/warning message
   * @return Error message string
   */
  std::string get_last_message() const;
  
  /**
   * Check if last operation had errors
   * @return true if errors occurred
   */
  bool has_errors() const;
  
  /**
   * Get simulation duration for profiling
   * @return CPU seconds used
   */
  double get_simulation_time() const;
  
  
  // ===== CONSTRUCTOR/DESTRUCTOR =====
  
  /**
   * Constructor
   */
  DaisyController();
  
  /**
   * Destructor
   */
  ~DaisyController();
  
  // Delete copy operations
  DaisyController(const DaisyController&) = delete;
  DaisyController& operator=(const DaisyController&) = delete;
  
  // Allow move operations
  DaisyController(DaisyController&&) noexcept;
  DaisyController& operator=(DaisyController&&) noexcept;

public:
  /** Direct access to the Daisy simulation object.
   *  For use by DaisyAPI extension methods — not intended for BMI callers.
   *  Public because DaisyAPI does not inherit DaisyController. */
  Daisy& daisy_ref ();
};

#endif // DAISY_BMI_H
