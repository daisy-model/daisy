#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>

namespace py = pybind11;
using namespace py::literals;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: call_python_function <python-module> <python-class> " \
      "[<constructor-parameter>]*" << std::endl;
    return 1;
  }
  py::scoped_interpreter guard{};

  auto py_module = py::module_::import(argv[1]);

  std::vector<std::string> parameters;
  for (std::size_t i = 3; i < argc; i++) {
    parameters.push_back(argv[i]);
  }
  py::object py_object = py_module.attr(argv[2])(parameters);

  std::vector<double> inputs {1, 10, 20, 30, 40};
  std::string domain = py_object.attr("domain").cast<std::string>();
  std::string range = py_object.attr("range").cast<std::string>();
  for (auto input : inputs) {
    double py_result = py_object(input).cast<double>();
    std::cout << "Input: " << input << " " << domain << std::endl
              << "Result: " << py_result << " " << range << std::endl;
  }
  return 0;
}
