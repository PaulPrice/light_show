#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "ndarray.h"
#include "ndarray/pybind11.h"

#include "light_show/LedController.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace light_show {
namespace {


void declareStripType(py::module &mod) {
    py::enum_<StripType> type(mod, "StripType");
    type.value("RGBW", StripType::RGBW);
    type.value("RBGW", StripType::RBGW);
    type.value("GRBW", StripType::GRBW);
    type.value("GBRW", StripType::GBRW);
    type.value("BRGW", StripType::BRGW);
    type.value("BGRW", StripType::BGRW);
    type.value("RGB", StripType::RGB);
    type.value("RBG", StripType::RBG);
    type.value("GRB", StripType::GRB);
    type.value("GBR", StripType::GBR);
    type.value("BRG", StripType::BRG);
    type.value("BGR", StripType::BGR);
    type.export_values();
}


void declareLedController(py::module &mod) {
    py::class_<LedController> cls(mod, "LedController");
    cls.def(py::init<int, int, StripType, unsigned int>(),
            "gpio"_a, "num"_a, "type"_a, "dma"_a=10);
    cls.def(py::init<ndarray::Array<int, 1, 1> const&, ndarray::Array<int, 1, 1> const&,
                     ndarray::Array<StripType, 1, 1> const&, unsigned int>(),
            "gpio"_a, "num"_a, "type"_a, "dma"_a=10);
    cls.def("render", &LedController::render, "wait"_a=true);
    cls.def("size", &LedController::size);
    cls.def("__len__", &LedController::size);
    cls.def("__getitem__", py::overload_cast<int>(&LedController::operator[]), "index"_a);
}


PYBIND11_MODULE(LedController, mod) {
    py::register_exception<Ws2811Exception>(mod, "Ws2811Exception", PyExc_RuntimeError);
    declareStripType(mod);
    declareLedController(mod);
}

}  // anonymous namespace
}  // namespace light_show
