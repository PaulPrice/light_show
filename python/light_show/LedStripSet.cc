#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "ndarray.h"
#include "ndarray/pybind11.h"

#include "light_show/LedStripSet.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace light_show {
namespace {


void declareLedStripSet(py::module &mod) {
    py::class_<LedStripSet> cls(mod, "LedStripSet");
    cls.def_property("red", py::overload_cast<>(&LedStripSet::getRed), &LedStripSet::setRed);
    cls.def_property("green", py::overload_cast<>(&LedStripSet::getGreen), &LedStripSet::setGreen);
    cls.def_property("blue", py::overload_cast<>(&LedStripSet::getBlue), &LedStripSet::setBlue);
    cls.def("__getitem__", py::overload_cast<int>(&LedStripSet::operator[]));
    cls.def("get", py::overload_cast<int>(&LedStripSet::get));
    cls.def("set", py::overload_cast<int, ColorRGB const&>(&LedStripSet::set));
    cls.def("set", py::overload_cast<int, ColorHSV const&>(&LedStripSet::set));
    cls.def("set", py::overload_cast<int, Pixel, Pixel, Pixel>(&LedStripSet::set));
    cls.def("__len__", &LedStripSet::size);
    cls.def("size", &LedStripSet::size);
    cls.def("isOn", &LedStripSet::isOn);
    cls.def("fill", py::overload_cast<Pixel, Pixel, Pixel>(&LedStripSet::fill));
    cls.def("fill", py::overload_cast<ColorRGB const&>(&LedStripSet::fill));
    cls.def("clear", &LedStripSet::clear);
    cls.def("brightness", &LedStripSet::brightness);
    cls.def("getHSV", py::overload_cast<int>(&LedStripSet::getHSV, py::const_));
    cls.def("getHSV", py::overload_cast<>(&LedStripSet::getHSV, py::const_));
    cls.def("setHSV", py::overload_cast<ndarray::Array<float, 2, 1> const&>(&LedStripSet::setHSV),
            "hsv"_a);
    cls.def("setHSV", py::overload_cast<ndarray::Array<float, 1, 1> const&,
                                        ndarray::Array<float, 1, 1> const&,
                                        ndarray::Array<float, 1, 1> const&>(&LedStripSet::setHSV),
            "hue"_a, "saturation"_a, "value"_a);
}


PYBIND11_MODULE(LedStripSet, mod) {
    declareLedStrip(mod);
}

}  // anonymous namespace
}  // namespace light_show
