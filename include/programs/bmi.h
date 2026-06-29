// daisy_bmi.h -- BMI (Basic Model Interface) wrapper for Daisy
//
// Implements the BMI 2.0 standard so that Daisy can be driven by
// any BMI-aware framework.
//
// Reference: https://bmi.readthedocs.io/en/stable/

#ifndef BMI_H
#define BMI_H

#include <string>
#include <vector>
#include "programs/daisy_bmi.h"

class Daisy;  // forward declaration for protected daisy() accessor

/**
 * @class BMI
 * @brief BMI 2.0 wrapper around DaisyBMI
 *
 * Variable names follow a "<category>_<quantity>" convention.
 *
 * Input variables (set_value):
 *   "groundwater__depth"          [cm]  - groundwater table depth
 *   "irrigation__rate"            [mm/day]
 *   "land_surface__air_temperature" [degC]  (if exposed by Daisy)
 *
 * Output variables (get_value):
 *   "soil_water__recharge_rate"   [mm/day]
 *   "land_surface__evapotranspiration_rate" [mm/day]
 *   "groundwater__depth"          [cm]
 *   "soil_water__transpiration_rate" [mm/day]
 *   "soil_water__evaporation_rate"   [mm/day]
 *   "soil_water__drainage_rate"      [mm/day]
 *   "soil_water__runoff_rate"        [mm/day]
 *   "vegetation__leaf_area_index"    [-]
 *   "vegetation__root_depth"         [cm]
 *   "vegetation__aboveground_biomass" [g/m2]
 *   "soil_water__content"            [m3/m3]  (first layer)
 */
class BMI
{
public:
  // ===== BMI LIFECYCLE =====

  /** Initialize model from a .dai config file. */
  void initialize(const std::string& config_file);

  /** Advance model by one timestep (dt = get_time_step()). */
  void update();

  /** Advance model to a specific time (in days since start). */
  void update_until(double time);

  /** Finalize and release all resources. */
  void finalize();

  // ===== BMI INFORMATION =====

  std::string get_component_name() const;
  std::vector<std::string> get_input_var_names() const;
  std::vector<std::string> get_output_var_names() const;

  // ===== BMI TIME =====

  double get_start_time() const;    // always 0.0
  double get_end_time() const;      // NaN = open-ended
  double get_current_time() const;  // days since simulation start
  double get_time_step() const;     // in days (e.g. 1/24 = hourly)
  std::string get_time_units() const;  // "d"

  // ===== BMI VARIABLE INFO =====

  std::string get_var_type(const std::string& name) const;    // "double"
  std::string get_var_units(const std::string& name) const;
  int         get_var_itemsize(const std::string& name) const; // sizeof(double)
  int         get_var_nbytes(const std::string& name) const;   // 1 * sizeof(double)
  std::string get_var_location(const std::string& name) const; // "none" (scalar)
  int         get_var_grid(const std::string& name) const;     // 0 (single-cell)

  // ===== BMI GRID INFO (single-cell, scalar grid) =====

  int         get_grid_rank(int grid) const;   // 0
  int         get_grid_size(int grid) const;   // 1
  std::string get_grid_type(int grid) const;   // "scalar"

  // ===== BMI GET/SET =====

  /** Get scalar output value by variable name into dest[0]. */
  void get_value(const std::string& name, double* dest) const;

  /** Set scalar input value by variable name from src[0]. */
  void set_value(const std::string& name, const double* src);

  // ===== CONSTRUCTOR / DESTRUCTOR =====
  BMI();
  ~BMI();

private:
  DaisyBMI ctrl_;
  double start_time_days_;   // always 0
  double current_time_days_; // updated each update()
  double dt_days_;           // timestep size in days

  static const std::vector<std::string> INPUT_VARS;
  static const std::vector<std::string> OUTPUT_VARS;

  double get_output_value(const std::string& name) const;

protected:
  /** Direct access to the Daisy simulation object for use by DaisyAPI
   *  extension methods.  Same Daisy& that DaisyBMI uses internally. */
  Daisy& daisy ();
};

#endif // DAISY_BMI_H
