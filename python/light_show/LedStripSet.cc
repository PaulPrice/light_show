#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "ndarray.h"
#include "ndarray/pybind11.h"

#include "light_show/python.h"
#include "light_show/LedStripSet.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace light_show {
namespace {


void declareLedStripSet(py::module &mod) {
    py::class_<LedStripSet> cls(mod, "LedStripSet");
    cls.def(py::init<LedStripSet::Collection>(), "strips"_a);
    cls.def(py::init<std::vector<LedStripSet> const&>(), "stripSets"_a);
    cls.def_property("red", py::overload_cast<>(&LedStripSet::getRed), &LedStripSet::setRed);
    cls.def_property("green", py::overload_cast<>(&LedStripSet::getGreen), &LedStripSet::setGreen);
    cls.def_property("blue", py::overload_cast<>(&LedStripSet::getBlue), &LedStripSet::setBlue);
    cls.def("__getitem__", py::overload_cast<LedStripSet::Index>(&LedStripSet::operator[]));
    cls.def("__getitem__", getFromSlice<LedStripSet>);
    cls.def("__setitem__", py::overload_cast<LedStripSet::Index, ColorRGB const&>(&LedStripSet::set));
    cls.def("__setitem__", py::overload_cast<LedStripSet::Index, ColorHSV const&>(&LedStripSet::set));
    cls.def("__setitem__", py::overload_cast<LedStripSet::Index, Pixel, Pixel, Pixel>(&LedStripSet::set));
    cls.def("__setitem__", setFromSlice<LedStripSet, ColorRGB>);
    cls.def("__setitem__", setFromSlice<LedStripSet, ColorRGBRef>);
    cls.def("__setitem__", setFromSlice<LedStripSet, ColorHSV>);
    cls.def("__setitem__", setFromIndexArray<LedStripSet, ColorRGB>, "indices"_a, "rhs"_a);
    cls.def("__setitem__", setFromIndexArray<LedStripSet, ColorRGBRef>, "indices"_a, "rhs"_a);
    cls.def("__setitem__", setFromIndexArray<LedStripSet, ColorHSV>, "indices"_a, "rhs"_a);
    cls.def("__len__", &LedStripSet::size);
    cls.def("__iter__", [](LedStripSet & self) { return py::make_iterator(self.begin(), self.end()); },
            py::keep_alive<0, 1>());
    cls.def("size", &LedStripSet::size);
    cls.def("isOn", &LedStripSet::isOn);
    cls.def("fill", py::overload_cast<Pixel, Pixel, Pixel>(&LedStripSet::fill));
    cls.def("fill", py::overload_cast<ColorRGB const&>(&LedStripSet::fill));
    cls.def("fill", [](LedStripSet & self, ColorHSV const& hsv) { self.fill(hsv); });
    cls.def("clear", &LedStripSet::clear);
    cls.def("left", &LedStripSet::left, "num"_a=1, "fill"_a=BLACK);
    cls.def("right", &LedStripSet::right, "num"_a=1, "fill"_a=BLACK);
    cls.def("brightness", &LedStripSet::brightness);
    cls.def("getHSV", py::overload_cast<LedStripSet::Index>(&LedStripSet::getHSV, py::const_));
    cls.def("getHSV", py::overload_cast<>(&LedStripSet::getHSV, py::const_));
    cls.def("setHSV", py::overload_cast<ndarray::Array<float, 2, 1> const&>(&LedStripSet::setHSV),
            "hsv"_a);
    cls.def("setHSV", py::overload_cast<ndarray::Array<float, 1, 1> const&,
                                        ndarray::Array<float, 1, 1> const&,
                                        ndarray::Array<float, 1, 1> const&>(&LedStripSet::setHSV),
            "hue"_a, "saturation"_a, "value"_a);
}


PYBIND11_MODULE(LedStripSet, mod) {
    py::module_::import("light_show.ConcatenatedArrays");
    declareLedStripSet(mod);
}

}  // anonymous namespace
}  // namespace light_show
