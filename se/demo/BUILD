cc_binary(
    name = "demo",
    srcs = glob(
        [
            "demo.cc",
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
