from setuptools import setup
from setuptools.extension import Extension
from os import path
cur_dir, _ = path.split(__file__)

setup_opts = dict(
    name = "mvdtool",
    version = 1.0,
    author = "Blue Brain Project, EPFL",
    ext_modules = [Extension('mvdtool',
                             sources=['mvdtool.cpp'],
                             include_dirs=[path.join(cur_dir,"../include"), path.join(cur_dir,"../deps/highfive/include")],
                             extra_compile_args=["-std=c++11"])
                   ]
)

if __name__ == "__main__":
    setup(**setup_opts)
