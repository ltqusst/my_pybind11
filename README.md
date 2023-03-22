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
