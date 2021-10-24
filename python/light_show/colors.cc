#include <sstream>
#include "pybind11/pybind11.h"

#include "light_show/python.h"
#include "light_show/colors.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace light_show {
namespace {


void declareColorRGB(py::module &mod) {
    py::class_<ColorRGB> cls(mod, "ColorRGB");
    cls.def(py::init<Pixel, Pixel, Pixel>(), "red"_a, "green"_a, "blue"_a);
    cls.def(py::init<ColorRGB>());
    cls.def(py::init<ColorRGBRef>());
    cls.def(py::init<ColorHSV>());
    cls.def("set", py::overload_cast<ColorRGB const&>(&ColorRGB::operator=), "other"_a);
    cls.def("set", py::overload_cast<ColorHSV const&>(&ColorRGB::operator=), "hsv"_a);
    cls.def_readwrite("red", &ColorRGB::red);
    cls.def_readwrite("green", &ColorRGB::green);
    cls.def_readwrite("blue", &ColorRGB::blue);
    declareStringify(cls, "__str__");
    declareStringify(cls, "__repr__");
}


void declareColorRGBRef(py::module &mod) {
    py::class_<ColorRGBRef> cls(mod, "ColorRGBRef");
    cls.def("set", py::overload_cast<ColorRGB const&>(&ColorRGBRef::operator=), "other"_a);
    cls.def("set", py::overload_cast<ColorRGBRef const&>(&ColorRGBRef::operator=), "other"_a);
    cls.def("set", py::overload_cast<ColorHSV const&>(&ColorRGBRef::operator=), "hsv"_a);
    cls.def_property("red", [](ColorRGBRef const& self) { return self.red; },
                     [](ColorRGBRef & self, Pixel red) { self.red = red; });
    cls.def_property("green", [](ColorRGBRef const& self) { return self.green; },
                     [](ColorRGBRef & self, Pixel green) { self.green = green; });
    cls.def_property("blue", [](ColorRGBRef const& self) { return self.blue; },
                     [](ColorRGBRef & self, Pixel blue) { self.blue = blue; });
    declareStringify(cls, "__str__");
    declareStringify(cls, "__repr__");
}


void declareColorHSV(py::module &mod) {
    py::class_<ColorHSV> cls(mod, "ColorHSV");
    cls.def(py::init<float, float, float>(), "hue"_a, "saturation"_a, "value"_a);
    cls.def(py::init<ColorRGB>());
    cls.def(py::init<ColorRGBRef>());
    cls.def(py::init<ColorHSV>());
    cls.def("set", py::overload_cast<ColorRGB const&>(&ColorHSV::operator=), "rgb"_a);
    cls.def("set", py::overload_cast<ColorHSV const&>(&ColorHSV::operator=), "other"_a);
    cls.def("toRGB", &ColorHSV::toRGB);
    cls.def_readwrite("hue", &ColorHSV::hue);
    cls.def_readwrite("saturation", &ColorHSV::saturation);
    cls.def_readwrite("sat", &ColorHSV::saturation);
    cls.def_readwrite("value", &ColorHSV::value);
    declareStringify(cls, "__str__");
    declareStringify(cls, "__repr__");
}


PYBIND11_MODULE(colors, mod) {
    declareColorRGB(mod);
    declareColorRGBRef(mod);
    declareColorHSV(mod);
    mod.attr("WHITE") = py::cast(WHITE);
    mod.attr("BLACK") = py::cast(BLACK);
    mod.attr("RED") = py::cast(RED);
    mod.attr("ORANGE") = py::cast(ORANGE);
    mod.attr("YELLOW") = py::cast(YELLOW);
    mod.attr("GREEN") = py::cast(GREEN);
    mod.attr("BLUE") = py::cast(BLUE);
    mod.attr("MAGENTA") = py::cast(MAGENTA);
    mod.attr("CYAN") = py::cast(CYAN);
    mod.attr("PINK") = py::cast(PINK);
    mod.attr("BROWN") = py::cast(BROWN);
    mod.attr("SKY") = py::cast(SKY);
    mod.attr("AQUA") = py::cast(AQUA);
    mod.attr("CRIMSON") = py::cast(CRIMSON);
    mod.attr("GOLD") = py::cast(GOLD);
}

}  // anonymous namespace
}  // namespace light_show
