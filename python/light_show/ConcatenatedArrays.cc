#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "ndarray.h"
#include "ndarray/pybind11.h"

#include "light_show/config.h"
#include "light_show/python.h"
#include "light_show/ConcatenatedArrays.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace light_show {
namespace {


#define DECLARE_OPERATOR(INSTANCE, TYPE, PYNAME, FUNCTION) \
    (INSTANCE).def(PYNAME, py::overload_cast<TYPE>(FUNCTION), "scalar"_a); \
    (INSTANCE).def(PYNAME, \
                   py::overload_cast<typename ConcatenatedArrays<TYPE>::Array::Shallow const&>(FUNCTION), \
                   "array"_a); \
    (INSTANCE).def(PYNAME, py::overload_cast<ConcatenatedArrays<TYPE> const&>(FUNCTION), "other"_a);


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
            return Class(container);
        }), "arrays"_a);
    cls.def("__len__", &Class::size);
    cls.def("__getitem__", [](Class & self, typename Class::Index index) { return self[index]; });
    cls.def("__getitem__", getFromSlice<Class>);
    cls.def("__getitem__",
            py::overload_cast<ndarray::Array<typename Class::Index, 1, 1> const&>(&Class::operator[]),
            "indices"_a);
    cls.def("__setitem__", [](Class & self, typename Class::Index index, T scalar) { self[index] = scalar; });
    cls.def("__setitem__", setFromSlice<Class, T>, "slice"_a, "scalar"_a);
    cls.def("__setitem__", [](Class & self, py::slice const& slice, ndarray::Array<T, 1, 0> const& array) {
        getFromSlice(self, slice) = array.deep();
    });
    cls.def("__setitem__", [](Class & self, py::slice const& slice, Class const& other) {
        getFromSlice(self, slice) = other;
    });
    cls.def("__setitem__", setFromIndexArray<Class, T>, "indices"_a, "rhs"_a);
    cls.def("__setitem__",
            [](Class & self, ndarray::Array<typename Class::Index, 1, 1> const& indices,
               typename Class::Array::Shallow const& rhs) {
        if (indices.size() != rhs.size()) {
            throw std::length_error("Size mismatch");
        }
        for (typename Class::Size ii = 0; ii < rhs.size(); ++ii) {
            self[indices[ii]] = rhs[ii];
        }
    }, "indices"_a, "rhs"_a);
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
