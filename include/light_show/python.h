#ifndef LIGHT_SHOW_PYTHON_H
#define LIGHT_SHOW_PYTHON_H

#include <string>
#include <sstream>
#include "ndarray_fwd.h"

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace light_show {

template <class PyClass>
void declareStringify(PyClass &cls, std::string const &method) {
    cls.def(method.c_str(), [](typename PyClass::type const &self) {
        std::ostringstream os;
        os << self;
        return os.str();
    });
}


template <typename LHS, typename RHS>
void setFromSlice(LHS & self, py::slice const& slice, RHS const& rhs) {
    std::size_t start, stop, step, size;
    if (!slice.compute(self.size(), &start, &stop, &step, &size)) {
        throw std::length_error("Unable to compute slice");
    }
    auto const end = self.begin() + stop;
    for (auto iter = self.begin() + start; iter != end; iter += step) {
        *iter = rhs;
    }
}


template <typename LHS, typename RHS>
void setFromIndexArray(LHS & self, ndarray::Array<typename LHS::Index, 1, 1> const& indices, RHS const& rhs) {
    for (auto index : indices) {
        self[index] = rhs;
    }
}


}  // namespace light_show

#endif  // include guard
