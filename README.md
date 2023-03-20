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
