#include <pybind11/pybind11.h>

int add(int i, int j) {
    return i + j;
}

/*
https://docs.python.org/3/extending/building.html

The PYBIND11_MODULE() macro creates `initialization function` require by CPython extension.

    extern/pybind11/include/pybind11/detail/common.h

::pybind11::module_::module_def pybind11_module_def_example1;
static void pybind11_init_example1(::pybind11::module_ &);
extern "C" PyObject * PyInit_example1() {
    // call PyModule_Create() internally
    auto m = ::pybind11::module_::create_extension_module(
                    PYBIND11_TOSTRING(name), nullptr, &pybind11_module_def_example1));
    try {
        pybind11_init_example1(m);
        return m.ptr();
    }
}

void pybind11_init_example1(::pybind11::module_ & (m)) {
    m.doc()=...
    m.def(...)
}

The second argument (m) defines a variable of type py::module_ which
is the main interface for creating bindings.

============= inside python
>>> import example1
>>> type(example1)
<class 'module'>
>>> hasattr(example1, '__doc__')
True
>>> getattr(example1, '__doc__')        fundamental API similar to C
'pybind11 example1 plugin'
>>> example1.__doc__                    python syntax sugar
'pybind11 example1 plugin'
>>> setattr(example1, '__doc__', '111') fundamental API similar to C
>>> example1.__doc__='111'              python syntax sugar
*/
PYBIND11_MODULE(example1, m) {
    // doc() creates an accessor object which, when being assigned, CPython APIs will be called.
    //
    // C++ Type  C++ syntax    pybind11_warpper    CPython ABI          python magic method
    //----------------------------------------------------------------------------------------
    // object_api::operator[]  item_accessor     PyObject_GetItem       __getitem__ 
    //                                           PyObject_SetItem       __setitem__
    // object_api::attr(key)   obj_attr_accessor PyObject_SetAttr
    // object_api::attr(key)   str_attr_accessor PyObject_SetAttrString
    // sequence::operator[]    sequence_accessor PySequence_SetItem
    //                                             PySequence_GetItem
    // list::operator[]        list_accessor     PyList_SetItem/PyList_GetItem
    // tuple::operator[]       tuple_accessor    PyTuple_SetItem/PyTuple_GetItem
    m.doc() = "pybind11 example1 plugin"; // optional module docstring

    //The method module_::def() generates binding code that exposes the add() function to Python.
    // calls PyModule_AddObject() internally

    // What truly amazing is how signature of add is parsed by pybind11 using meta programing

    m.def("add", &add, "A function that adds two numbers");
}
