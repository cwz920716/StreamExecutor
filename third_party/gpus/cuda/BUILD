licenses(["restricted"]) # MPL2, portions GPL v3, LGPL v3, BSD-like

package(default_visibility = ["//visibility:public"])

config_setting(
    name = "cuda_crosstool_condition",
    values = {"crosstool_top": "//third_party/gpus/crosstool"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "using_gcudacc",
    values = {
        "crosstool_top": "//third_party/gpus/crosstool",
        "copt": "--use_gcudacc",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "using_nvcc",
    values = {
        "crosstool_top": "//third_party/gpus/crosstool",
        "copt": "--use_nvcc",
    },
)

cc_library(
    name = "cuda_headers",
    hdrs = glob([
        "*.h",
        "**/*.h",
        "**/*.hpp",
    ]),
    includes = [".", "include"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cudart_static",
    srcs = [
        "lib64/libcudart_static.a",
    ],
    includes = ["include/"],
    linkopts = [
        "-ldl",
        "-lrt",
        "-lpthread",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cudart",
    srcs = [
        "lib64/libcudart.so.8.0",
    ],
    data = [
        "lib64/libcudart.so.8.0",
    ],
    includes = ["include/"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

cc_library(
    name = "cublas",
    srcs = [
        "lib64/libcublas.so.8.0",
    ],
    data = [
        "lib64/libcublas.so.8.0",
    ],
    includes = ["include/"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

cc_library(
    name = "cudnn",
    srcs = [
        "lib64/libcudnn.so.5",
    ],
    data = [
        "lib64/libcudnn.so.5",
    ],
    includes = ["include/"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

cc_library(
    name = "cufft",
    srcs = [
        "lib64/libcufft.so.8.0",
    ],
    data = [
        "lib64/libcufft.so.8.0",
    ],
    includes = ["include/"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

cc_library(
    name = "curand",
    srcs = [
        "lib64/libcurand.so.8.0",
    ],
    data = [
        "lib64/libcurand.so.8.0",
    ],
    includes = ["include/"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

cc_library(
    name = "cuda",
    deps = [
        ":cuda_headers",
        ":cudart",
        ":cublas",
        ":cudnn",
    ],
    visibility = ["//visibility:public"],
)
