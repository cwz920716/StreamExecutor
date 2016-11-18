licenses(["restricted"]) # MPL2, portions GPL v3, LGPL v3, BSD-like

cc_library(
    name = "stream_executor",
    srcs = glob(
        [
            "*.cc",
            "lib/*.cc",
            "cuda/*.cc",
            "platform/*.cc",
            "platform/**/*.cc",
            "core/**/*.cc",
            "core/**/**/*.cc",
        ],
        exclude = [
            "**/*_test.cc",
        ],
    ),
    hdrs = glob([
        "*.h",
        "cuda/*.h",
        "lib/*.h",
        "lib/**/*.h",
        "platform/*.h",
        "platform/**/*.h",
        "core/**/*.h",
        "core/**/**/*.h",
    ]),
    data = [
        "//third_party/gpus/cuda:cublas",
        "//third_party/gpus/cuda:cudnn",
        "//third_party/gpus/cuda:cufft",
        "//third_party/gpus/cuda:curand",
    ],
    visibility = [
        "//visibility:public"
    ],
    deps = [
        "//third_party/gpus/cuda:cuda_headers",
        "//third_party/eigen3",
    ],
    linkopts = [
        "-ldl",
    ],
)
