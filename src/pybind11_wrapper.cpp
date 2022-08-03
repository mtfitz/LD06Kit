#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <LD06Kit/lidarkit.hpp>

namespace py = pybind11;

PYBIND11_MODULE(ld06kit, m)
{
    m.doc() = "LD06 lidar sensor interface for Python";
    
    py::class_<LidarPoint>(m, "LidarPoint")
        .def(py::init<float, float, int, int>())
        .def_readwrite("angle", &LidarPoint::angle)
        .def_readwrite("distance", &LidarPoint::distance)
        .def_readwrite("confidence", &LidarPoint::confidence)
        .def_readwrite("timestamp", &LidarPoint::timestamp);

    py::class_<LidarKit>(m, "LidarKit")
      .def(py::init<const std::string&, bool>(), py::arg("uri"), py::arg("debug_mode") = false)
        .def("start", &LidarKit::start)
        .def("stop", &LidarKit::stop)
        .def("get_points", &LidarKit::get_points);
}
