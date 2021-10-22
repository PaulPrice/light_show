#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "ndarray.h"
#include "ndarray/pybind11.h"

#include "light_show/LedStrip.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace light_show {
namespace {


void declareLedStrip(py::module &mod) {
    py::class_<LedStrip> cls(mod, "LedStrip");
    cls.def_property("red", py::overload_cast<>(&LedStrip::getRed), &LedStrip::setRed);
    cls.def_property("green", py::overload_cast<>(&LedStrip::getGreen), &LedStrip::setGreen);
    cls.def_property("blue", py::overload_cast<>(&LedStrip::getBlue), &LedStrip::setBlue);
    cls.def("__getitem__", py::overload_cast<LedStrip::Index>(&LedStrip::operator[]));
    cls.def("get", py::overload_cast<LedStrip::Index>(&LedStrip::get));
    cls.def("set", py::overload_cast<LedStrip::Index, ColorRGB const&>(&LedStrip::set));
    cls.def("set", py::overload_cast<LedStrip::Index, ColorHSV const&>(&LedStrip::set));
    cls.def("set", py::overload_cast<LedStrip::Index, Pixel, Pixel, Pixel>(&LedStrip::set));
    cls.def("__len__", &LedStrip::size);
    cls.def("size", &LedStrip::size);
    cls.def("isOn", &LedStrip::isOn);
    cls.def("fill", py::overload_cast<Pixel, Pixel, Pixel>(&LedStrip::fill));
    cls.def("fill", py::overload_cast<ColorRGB const&>(&LedStrip::fill));
    cls.def("clear", &LedStrip::clear);
    cls.def("brightness", &LedStrip::brightness);
    cls.def("getHSV", py::overload_cast<LedStrip::Index>(&LedStrip::getHSV, py::const_));
    cls.def("getHSV", py::overload_cast<>(&LedStrip::getHSV, py::const_));
    cls.def("setHSV", py::overload_cast<ndarray::Array<float, 2, 1> const&>(&LedStrip::setHSV),
            "hsv"_a);
    cls.def("setHSV", py::overload_cast<ndarray::Array<float, 1, 1> const&,
                                        ndarray::Array<float, 1, 1> const&,
                                        ndarray::Array<float, 1, 1> const&>(&LedStrip::setHSV),
            "hue"_a, "saturation"_a, "value"_a);
}


PYBIND11_MODULE(LedStrip, mod) {
    declareLedStrip(mod);
}

}  // anonymous namespace
}  // namespace light_show
