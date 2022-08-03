from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension

ext_modules = [
    Pybind11Extension(
        "ld06kit",
        sources = ["src/pybind11_wrapper.cpp"],
        library_dirs = ["/usr/local/lib"],
        libraries = ["ld06kit"],
        include_dirs = ["/usr/local/include"],
        extra_compile_args=["-std=c++17"],
    ),
]

setup(
    name="ld06kit",
    version="0.0.1",
    author="Michael Fitzgerald",
    author_email="mtf323@lehigh.edu",
    ext_modules=ext_modules
)
