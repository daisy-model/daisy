// api_bindings.cpp -- pybind11 bindings for DaisyBMI
//
// Exposes DaisyBMI to Python as the module "daisy_bmi".
//
// Compile (example, adjust paths):
//   g++ -O3 -Wall -shared -std=c++17 -fPIC
//       $(python3 -m pybind11 --includes)
//       daisy_api_bindings.cpp daisy_bmi.C daisy_python_controller.C
//       -o daisy_bmi$(python3-config --extension-suffix)
//       -L. -ldaisy_core
//
// Usage from Python:
//   import daisy_bmi
//   m = daisy_bmi.DaisyBMI()
//   m.initialize("myconfig.dai")
//   while m.get_current_time() < 365.0:
//       recharge = m.get_value("soil_water__recharge_rate")
//       m.set_value("groundwater__depth", 150.0)
//       m.update()
//   m.finalize()

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "programs/bmi.h"

namespace py = pybind11;

PYBIND11_MODULE(daisy_bmi, m)
{
  m.doc() = R"pbdoc(
    daisy_bmi - BMI 2.0 interface to the Daisy soil-crop-atmosphere model
    =======================================================================

    Example
    -------
    import daisy_bmi

    bmi = daisy_bmi.DaisyBMI()
    bmi.initialize("myconfig.dai")

    print("Component:", bmi.get_component_name())
    print("Time step:", bmi.get_time_step(), bmi.get_time_units())
    print("Inputs:",    bmi.get_input_var_names())
    print("Outputs:",   bmi.get_output_var_names())

    while bmi.get_current_time() < 365.0:
        # Pull outputs
        recharge = bmi.get_value("soil_water__recharge_rate")
        et       = bmi.get_value("land_surface__evapotranspiration_rate")

        # Push inputs
        bmi.set_value("groundwater__depth", 150.0)  # 150 cm below surface

        bmi.update()

    bmi.finalize()
  )pbdoc";

  py::class_<DaisyBMI>(m, "DaisyAPI")

    .def(py::init<>(), "Create a new Daisy BMI instance")

    // --- Lifecycle ---
    .def("initialize",    &DaisyBMI::initialize,    py::arg("config_file"),
         "Initialize from a .dai config file")
    .def("update",        &DaisyBMI::update,
         "Advance model by one timestep")
    .def("update_until",  &DaisyBMI::update_until,  py::arg("time"),
         "Advance model until time (days since start)")
    .def("finalize",      &DaisyBMI::finalize,
         "Finalize and release all resources")

    // --- Information ---
    .def("get_component_name",    &DaisyBMI::get_component_name)
    .def("get_input_var_names",   &DaisyBMI::get_input_var_names)
    .def("get_output_var_names",  &DaisyBMI::get_output_var_names)

    // --- Time ---
    .def("get_start_time",    &DaisyBMI::get_start_time,
         "Simulation start time [days]")
    .def("get_end_time",      &DaisyBMI::get_end_time,
         "Simulation end time [days] (NaN = open-ended)")
    .def("get_current_time",  &DaisyBMI::get_current_time,
         "Current simulation time [days since start]")
    .def("get_time_step",     &DaisyBMI::get_time_step,
         "Current timestep size [days]")
    .def("get_time_units",    &DaisyBMI::get_time_units,
         "Time unit string, e.g. 'd'")

    // --- Variable metadata ---
    .def("get_var_type",      &DaisyBMI::get_var_type,     py::arg("name"))
    .def("get_var_units",     &DaisyBMI::get_var_units,    py::arg("name"))
    .def("get_var_itemsize",  &DaisyBMI::get_var_itemsize, py::arg("name"))
    .def("get_var_nbytes",    &DaisyBMI::get_var_nbytes,   py::arg("name"))
    .def("get_var_location",  &DaisyBMI::get_var_location, py::arg("name"))
    .def("get_var_grid",      &DaisyBMI::get_var_grid,     py::arg("name"))

    // --- Grid metadata ---
    .def("get_grid_rank",  &DaisyBMI::get_grid_rank, py::arg("grid"))
    .def("get_grid_size",  &DaisyBMI::get_grid_size, py::arg("grid"))
    .def("get_grid_type",  &DaisyBMI::get_grid_type, py::arg("grid"))

    // --- Get / Set (scalar convenience wrappers) ---
    .def("get_value",
         [](const DaisyBMI& self, const std::string& name) -> double {
           // Allocate the correct-sized buffer; array vars (e.g.
           // soil_water__recharge_rate) hold one value per soil layer.
           // Writing N elements into a 1-element stack variable is UB and
           // caused WinError 10054 crashes via stack corruption.
           int n = self.get_var_nbytes(name) / static_cast<int>(sizeof(double));
           std::vector<double> buf(n);
           self.get_value(name, buf.data());
           return buf.back(); // scalar = bottom-layer value, matches get_output_value
         },
         py::arg("name"),
         "Get a scalar output value by BMI variable name")

    .def("set_value",
         [](DaisyBMI& self, const std::string& name, double value) {
           self.set_value(name, &value);
         },
         py::arg("name"), py::arg("value"),
         "Set a scalar input value by BMI variable name")

    .def("get_value_array",
     [](const DaisyBMI& self, const std::string& name) {
       int n = self.get_var_nbytes(name) / static_cast<int>(sizeof(double));
       py::array_t<double> arr(n);
       self.get_value(name, arr.mutable_data());
       return arr;
     },
     py::arg("name"),
     "Get an array output value as a numpy array of doubles");
}
