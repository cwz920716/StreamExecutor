cc_binary(
    name = "driver",
    srcs = glob(
        [
            "*.cc",
            "*.h",
        ],
        exclude = [
            "**/*_test.cc",
        ],
    ),
    deps = [
        "//se/stream_executor",
    ],
    linkopts = [
        "-lprotobuf",
    ],
)
