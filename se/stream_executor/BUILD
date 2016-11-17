_library(
    name = "stream_executor",
    srcs = glob(
        [
            "*.cc",
            "lib/*.cc",
            "cuda/*.cc",
        ],
        exclude = [
            "**/*_test.cc",
        ],
    ),
    hdrs = glob([
        "*.h",
        "lib/*.h",
        "platform/**/*.h",
    ]),
    linkopts = [
        "-ldl",
    ],
)
