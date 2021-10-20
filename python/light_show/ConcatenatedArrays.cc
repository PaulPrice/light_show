#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "ndarray.h"
#include "ndarray/pybind11.h"

#include "light_show/ConcatenatedArrays.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace light_show {
namespace {


#define DECLARE_OPERATOR(INSTANCE, TYPE, PYNAME, FUNCTION) \
    (INSTANCE).def(PYNAME, py::overload_cast<TYPE>(FUNCTION), "scalar"_a); \
    (INSTANCE).def(PYNAME, py::overload_cast<typename ConcatenatedArrays<TYPE>::Array const&>(FUNCTION), \
                   "array"_a); \
    (INSTANCE).def(PYNAME, py::overload_cast<ConcatenatedArrays<TYPE> const&>(FUNCTION), "other"_a);


template <typename T>
void declareConcatenatedArrays(py::module &mod, const char* suffix) {
    using Class = ConcatenatedArraysRef<T>;  // Everything is writable in python
    py::class_<Class> cls(mod, (std::string("ConcatenatedArrays") + suffix).c_str());
    cls.def("__len__", &Class::size);
    cls.def("__getitem__", [](Class & self, typename Class::Index index) { return self[index]; });
    cls.def("__getitem__", [](Class & self, py::slice const& slice) {
        typename Class::Index start, stop, step, size;
        if (!slice.compute(self.size(), &start, &stop, &step, &size)) {
            throw std::length_error("Unable to compute slice");
        }
        if (size == self.size()) {
            return self;
        }
        return self.slice(start, stop, step);
    });
    cls.def_property_readonly("array", [](Class const& self) { return typename Class::Array(self); });
    cls.def_property_readonly("size", &Class::size);
    DECLARE_OPERATOR(cls, T, "__iadd__", &Class::operator+=);
    DECLARE_OPERATOR(cls, T, "__isub__", &Class::operator-=);
    DECLARE_OPERATOR(cls, T, "__imul__", &Class::operator*=);
    DECLARE_OPERATOR(cls, T, "__itruediv__", &Class::operator/=);
}


PYBIND11_MODULE(ConcatenatedArrays, mod) {
    declareConcatenatedArrays<int>(mod, "I");
    declareConcatenatedArrays<double>(mod, "D");
}

}  // anonymous namespace
}  // namespace light_show
