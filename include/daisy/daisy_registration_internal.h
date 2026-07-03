// daisy_registration_internal.h -- Internal Daisy registration declarations.
//
// Copyright 2026 The Daisy Authors.
//
// This file is part of Daisy.
//
// Daisy is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// Daisy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU Lesser Public License
// along with Daisy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef DAISY_REGISTRATION_INTERNAL_H
#define DAISY_REGISTRATION_INTERNAL_H

void register_column_models ();
void register_column_standard_models ();
void register_condition_models ();
void register_condition_BBCH_models ();
void register_condition_boolean_models ();
void register_condition_crop_models ();
void register_condition_daisy_state_models ();
void register_condition_extern_models ();
void register_condition_logic_models ();
void register_condition_soil_models ();
void register_condition_time_models ();
void register_condition_walltime_models ();
void register_condition_weather_models ();
void register_action_models ();
void register_action_BBCH_models ();
void register_action_activity_models ();
void register_action_crop_models ();
void register_action_extern_models ();
void register_action_fertilize_models ();
void register_action_harvest_models ();
void register_action_irrigate_models ();
void register_action_lisp_models ();
void register_action_markvand_models ();
void register_action_message_models ();
void register_action_repeat_models ();
void register_action_sow_models ();
void register_action_spray_models ();
void register_action_stop_models ();
void register_action_surface_models ();
void register_action_table_models ();
void register_action_tillage_models ();
void register_action_wait_models ();
void register_action_while_models ();
void register_action_with_models ();
void register_daisy_program_models ();
void register_daisy_time_models ();
void register_fetch_models ();
void register_fetch_pretty_models ();
void register_groundwater_models ();
void register_groundwater_aquitard_models ();
void register_groundwater_deep_models ();
void register_groundwater_extern_models ();
void register_groundwater_file_models ();
void register_groundwater_fixed_models ();
void register_groundwater_flux_models ();
void register_groundwater_lysimeter_models ();
void register_groundwater_source_models ();
void register_groundwater_static_models ();
void register_harvest_models ();
void register_irrigation_models ();
void register_log_models ();
void register_log_checkpoint_models ();
void register_log_extern_models ();
void register_log_harvest_models ();
void register_log_select_models ();
void register_log_table_models ();
void register_output_models ();
void register_select_models ();
void register_select_array_models ();
void register_select_content_models ();
void register_select_flow_models ();
void register_select_index_models ();
void register_select_number_models ();
void register_select_quiver_models ();
void register_select_value_models ();
void register_select_volume_models ();
void register_summary_models ();
void register_summary_balance_models ();
void register_summary_fractiles_models ();
void register_summary_Rsqr_models ();
void register_summary_RsqrW_models ();
void register_summary_simple_models ();
void register_timestep_models ();

#endif // DAISY_REGISTRATION_INTERNAL_H
