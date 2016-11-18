cc_library(
    name = "stream_executor",
    srcs = glob(
        [
            "*.cc",
            "lib/*.cc",
            "cuda/*.cc",
            "platform/**/*.cc",
            "lib/**/*.cc",
        ],
        exclude = [
            "**/*_test.cc",
        ],
    ),
    hdrs = glob([
        "*.h",
        "lib/*.h",
        "cuda/*.h",
        "lib/**/*.h",
        "platform/**/*.h",
        "eigen3/**/*",
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
