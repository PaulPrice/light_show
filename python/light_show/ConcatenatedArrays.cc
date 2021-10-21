#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "ndarray.h"
#include "ndarray/pybind11.h"

#include "light_show/config.h"
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
ConcatenatedArraysRef<T> getSlice(ConcatenatedArraysRef<T> & arrays, py::slice const& slice) {
    typename ConcatenatedArraysRef<T>::Index start, stop, step, size;
    if (!slice.compute(arrays.size(), &start, &stop, &step, &size)) {
        throw std::length_error("Unable to compute slice");
    }
    if (size == arrays.size()) {
        return arrays;
    }
    return arrays.slice(start, stop, step);
}


template <class PyClass>
void declareStringify(PyClass &cls, std::string const &method) {
    cls.def(method.c_str(), [](typename PyClass::type const &self) {
        std::ostringstream os;
        os << self;
        return os.str();
    });
}


template <typename T>
void declareConcatenatedArrays(py::module &mod, const char* suffix) {
    using Class = ConcatenatedArraysRef<T>;  // Everything is writable in python
    py::class_<Class> cls(mod, (std::string("ConcatenatedArrays") + suffix).c_str());
    cls.def(py::init(
        [](std::vector<ndarray::Array<T, 1, 0>> & arrays) {
            typename Class::Container container;
            container.reserve(arrays.size());
            for (auto & arr : arrays) {
                container.emplace_back(arr.deep());
            }
            return Class(std::move(container));
        }), "arrays"_a);
    cls.def("__len__", &Class::size);
    cls.def("__getitem__", [](Class & self, typename Class::Index index) { return self[index]; });
    cls.def("__getitem__", [](Class & self, py::slice const& slice) { return getSlice(self, slice); });
    cls.def("__setitem__", [](Class & self, typename Class::Index index, T scalar) { self[index] = scalar; });
    cls.def("__setitem__", [](Class & self, py::slice const& slice, T scalar) {
        getSlice(self, slice) = scalar;
    });
    cls.def("__setitem__", [](Class & self, py::slice const& slice, ndarray::Array<T, 1, 0> const& array) {
        getSlice(self, slice) = array.deep();
    });
    cls.def("__setitem__", [](Class & self, py::slice const& slice, Class const& other) {
        getSlice(self, slice) = other;
    });
    cls.def_property_readonly("array", [](Class const& self) {
        return typename Class::Array(self).shallow();
    });
    declareStringify(cls, "__str__");
    declareStringify(cls, "__repr__");
    cls.def_property_readonly("size", &Class::size);
    DECLARE_OPERATOR(cls, T, "__iadd__", &Class::operator+=);
    DECLARE_OPERATOR(cls, T, "__isub__", &Class::operator-=);
    DECLARE_OPERATOR(cls, T, "__imul__", &Class::operator*=);
    DECLARE_OPERATOR(cls, T, "__itruediv__", &Class::operator/=);
}


PYBIND11_MODULE(ConcatenatedArrays, mod) {
    declareConcatenatedArrays<Pixel>(mod, "");
}

}  // anonymous namespace
}  // namespace light_show
