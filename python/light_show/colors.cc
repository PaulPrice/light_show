#include "pybind11/pybind11.h"

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
}


void declareColorRGBRef(py::module &mod) {
    py::class_<ColorRGBRef> cls(mod, "ColorRGBRef");
    cls.def("set", py::overload_cast<ColorRGB const&>(&ColorRGBRef::operator=), "other"_a);
    cls.def("set", py::overload_cast<ColorRGBRef const&>(&ColorRGBRef::operator=), "other"_a);
    cls.def("set", py::overload_cast<ColorHSV const&>(&ColorRGBRef::operator=), "hsv"_a);
    cls.def_readwrite("red", &ColorRGB::red);
    cls.def_readwrite("green", &ColorRGB::green);
    cls.def_readwrite("blue", &ColorRGB::blue);
}


void declareColorHSV(py::module &mod) {
    py::class_<ColorRGB> cls(mod, "ColorHSV");
    cls.def(py::init<float, float, float>(), "hue"_a, "saturation"_a, "value"_a);
    cls.def(py::init<ColorRGB>());
    cls.def(py::init<ColorRGBRef>());
    cls.def(py::init<ColorHSV>());
    cls.def("set", py::overload_cast<ColorRGB const&>(&ColorHSV::operator=), "rgb"_a);
    cls.def("set", py::overload_cast<ColorHSV const&>(&ColorHSV::operator=), "other"_a);
    cls.def_readwrite("red", &ColorRGB::red);
    cls.def_readwrite("green", &ColorRGB::green);
    cls.def_readwrite("blue", &ColorRGB::blue);
}


PYBIND11_PLUGIN(colors) {
    py::module mod("colors");
    declareColorRGB(mod);
    declareColorRGBRef(mod);
    declareColorHSV(mod);
    py::globals()["WHITE"] = py::cast(WHITE);
    py::globals()["BLACK"] = py::cast(BLACK);
    py::globals()["RED"] = py::cast(RED);
    py::globals()["ORANGE"] = py::cast(ORANGE);
    py::globals()["YELLOW"] = py::cast(YELLOW);
    py::globals()["GREEN"] = py::cast(GREEN);
    py::globals()["BLUE"] = py::cast(BLUE);
    py::globals()["MAGENTA"] = py::cast(MAGENTA);
    py::globals()["CYAN"] = py::cast(CYAN);
    py::globals()["PINK"] = py::cast(PINK);
    py::globals()["BROWN"] = py::cast(BROWN);
    py::globals()["SKY"] = py::cast(SKY);
    py::globals()["AQUA"] = py::cast(AQUA);
    py::globals()["CRIMSON"] = py::cast(CRIMSON);
    py::globals()["GOLD"] = py::cast(GOLD);
    return mod.ptr();
}

}  // anonymous namespace
}  // namespace light_show
