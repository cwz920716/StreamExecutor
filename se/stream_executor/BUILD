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
    ]),
    visibility = [
        "//visibility:public"
    ],
    deps = [
        "//third_party/gpus/cuda:cuda_headers",
    ],
    linkopts = [
        "-ldl",
    ],
)
