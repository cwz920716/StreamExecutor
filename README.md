# StreamExecutor

I start this project since I need a standalone StreamExecutor runtime library (https://github.com/henline/streamexecutordoc). However, at the time of writing, StreamExecutor has not been open-sourced (or I haven't found it) so I decide to make my own library. The code will be mostly ported form tensorflow/stream_executor but I want to make it a standalone library for my own purpose.

To build this project, it assumes you have bazel (https://bazel.build/versions/master/docs/install.html) and Google Protocol Buffer (https://github.com/google/protobuf/blob/master/src/README.md) installed system wide. 

This project will stay sync with Tensorflow tagged releases to make sure it is up-to-date. For now, it is with Tensorflow v0.11.0. However, it may be deprecated as soon as a better alternative (e.g., Google's official StreamExecutor release) is available. 

Note: This project is only for in-house research purpose and all code/licenses are from Google's Tensorflow project unless stated otherwise. 
