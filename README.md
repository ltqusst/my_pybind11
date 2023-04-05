# my_pybind11
Learn how to use pybind11

## add pybind11 to project

https://pybind11.readthedocs.io/en/latest/installing.html

```bash
$ mkdir extern
$ git submodule add -b stable https://github.com/pybind/pybind11.git extern/pybind11
$ git submodule update --init
```

## integrate pybind11 into cmake

https://pybind11.readthedocs.io/en/latest/compiling.html#building-with-cmake

`add_subdirectory(extern/pybind11)` introduced a macro
[`pybind11_add_module`](https://pybind11.readthedocs.io/en/latest/compiling.html#pybind11-add-module), which is then used to add target and does all required cmake setup internally.

## python object as cpp class

https://pybind11.readthedocs.io/en/latest/advanced/pycpp/object.html

pybind11 wrapped many type exposed by [python C API](https://docs.python.org/3/c-api/index.html) into C++ classes.

check [pytypes.h](extern/pybind11/include/pybind11/pytypes.h) for how are they implemented.

they are wrapper, internally holding reference to the underlying object living in python runtime.

## Learn meta programming using cppinsights & godbolt

https://godbolt.org/
https://cppinsights.io/
https://github.com/andreasfertig/cppinsights

## What's happend in `m.def` ?

with following pybind11 C++ source:
```cpp
m.def("add", &add, "A function that adds two numbers");
```

This magic method "def" wraps a normal C++ function `add` into a python function object.
which we know is quite complex procedure that involves many details including [^1]

 - implement a wrapper function accepts `PyObject*` arguments;
 - parse & convert positional & keywords args from tuple & dict(using `PyArg_ParseTuple` for example)
   inside the wrapper and pass the result into C/C++ functions;
```c
static PyObject *
spam_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    return PyLong_FromLong(sts);
}
```
 - create `PyMethodDef` to define the wrapper to Python;
```c
static PyMethodDef SpamMethods[] = {
    ...
    {"system",  spam_system, METH_VARARGS, "Execute a shell command."},
    ...
    {NULL, NULL, 0, NULL}        /* Sentinel */
};
```
 - add it into `PyModuleDef` (using `PyModule_AddObject`)


Using the meta-programing technique, pybind11 magically does all above automatically for you.
`cpp_function` is the meta-class wrapper which wraps the function and exported to python.

all magic happens inside `cpp_function::initialize()`:

 - `cpp_function::dispatcher()` is the python wrapper that collect arg one by one
   from positional arguments tuple & keyword arguments dict, and store them in
   `struct function_call`.
 - `argument_loader<Args...>` template specialized for target function will
   load each arg into instances of `type_caster_base` in `argcasters` tuple.
   each of these instances has a convertion operator overloaded for particular
   target arg type
 - `argument_loader<Args...>::call` finally performs the invokation by passing
   all items in `argcasters` one by one to `f` callable.

## how __doc__ is set in `m.def` ?

what we saw in python
```python
>>> import example1
>>> type(example1.add)
<class 'builtin_function_or_method'>
>>> help(example1.add)
add(...) method of builtins.PyCapsule instance
    add(arg0: int, arg1: int) -> int
    
    A function that adds two numbers
```

how the python signature: `add(arg0: int, arg1: int) -> int` is generated and appended?

in method `cpp_function：：initialize`, `struct function_record` is created which is filled by expansion of a [template parameter pack](https://en.cppreference.com/w/cpp/language/parameter_pack), one overload for `const char *` type will fill the doc string property of `function_record`. later on, `initialize_generic(..)` create a [PyCapsule](https://docs.python.org/3/extending/extending.html?highlight=pycapsule_new#providing-a-c-api-for-an-extension-module) and [PyCFunction](https://docs.python.org/3/c-api/structures.html#implementing-functions-and-methods), and sets it's member `func->m_ml->ml_doc`.

the `builtin_function_or_method` type shows that `PyCFunction` is responsible for providing all real implementations for all methods of `example1.add` visible in python, it's unlike a general object.

```cpp
 process_attributes<Extra...>::init(extra..., rec);

// template parameter pack expansion calls overloads of process_attribute<>
// to parse and fills function_record r
static void init(const Args &...args, function_record *r) {
    PYBIND11_WORKAROUND_INCORRECT_MSVC_C4100(r);
    PYBIND11_WORKAROUND_INCORRECT_GCC_UNUSED_BUT_SET_PARAMETER(r);
    using expander = int[];
    (void) expander{
        0, ((void) process_attribute<typename std::decay<Args>::type>::init(args, r), 0)...};
}

// following particular overload sets doc property
template <>
struct process_attribute<const char *> : process_attribute_default<const char *> {
    static void init(const char *d, function_record *r) { r->doc = const_cast<char *>(d); }
    static void init(const char *d, type_record *r) { r->doc = d; }
};

initialize_generic(...) {
    ...
    rec->def = new PyMethodDef();
    ...
    capsule rec_capsule(unique_rec.release(),
    ...
    m_ptr = PyCFunction_NewEx(rec->def, rec_capsule.ptr(), scope_module.ptr());

    ...
    std::string signatures;
    // Install docstring if it's non-empty (when at least one option is enabled)
    func->m_ml->ml_doc
        = signatures.empty() ? nullptr : PYBIND11_COMPAT_STRDUP(signatures.c_str());

}

```


[^1]: https://docs.python.org/3/extending/extending.html