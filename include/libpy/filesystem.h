#pragma once

#include <filesystem>

#include "libpy/borrowed_ref.h"
#include "libpy/detail/python.h"
#include "libpy/from_object.h"
#include "libpy/scoped_ref.h"
#include "libpy/util.h"

/** Filesystem tools.
 */
namespace py::filesystem {

namespace detail {
template<typename T, bool cond>
constexpr bool defer_check = cond;
}  // namespace detail

/** Get an std::filesystem::path from an object implementing __fspath__

    @param ob Object implementing __fspath__.
    @return A std::filesystem::path
*/
// make this a template to defer the static_assert check until the function is
// used.
template<typename T = void>
std::filesystem::path fs_path(py::borrowed_ref<> ob) {
    static_assert(detail::defer_check<T, (PY_VERSION_HEX >= 0x0306)>,
                  "cannot use fs_path in Python older than 3.6");

    py::scoped_ref path_obj{PyOS_FSPath(ob.get())};

    if (!path_obj) {
        throw py::exception{};
    }
    // depending on the implementation of __fspath__ we can get str or bytes
    std::filesystem::path path;
    if (PyBytes_Check(path_obj)) {
        path = py::from_object<std::string>(path_obj);
    }
    else {
        path = py::util::pystring_to_string_view(path_obj);
    }

    return path;
}

}  // namespace py::filesystem