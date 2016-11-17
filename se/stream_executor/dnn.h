// Neural Net operation support for StreamExecutor instances.
//
// This is an abstract interface for a platform to optionally support common
// neural net operations; it accommodates implementations such as the cudnn
// library operations.

#ifndef TENSORFLOW_STREAM_EXECUTOR_DNN_H_
#define TENSORFLOW_STREAM_EXECUTOR_DNN_H_

#include "se/stream_executor/device_memory.h"
#include "se/stream_executor/lib/array_slice.h"
#include "se/stream_executor/lib/status.h"
#include "se/stream_executor/platform/logging.h"
#include "se/stream_executor/platform/port.h"

namespace perftools {
namespace gputools {

class Stream;

namespace dnn {

// Describes how an input or output layer's data is formatted.
// Specify int64 so there's no padding in BatchDescriptor.
enum class DataLayout : int64 {
  kYXDepthBatch = 0,  // Same as dist_belief::DF_DEPTH_MAJOR.
  kYXBatchDepth,      // Same as dist_belief::DF_BATCH_MAJOR.
  kBatchYXDepth,      // Same as run_brain output, and tensorflow's layout.
  kBatchDepthYX,      // cuDNN's NCHW layout, data laid out as image, feature,
                      // maps, rows, columns.
};

// Returns a string representation of the given data layout.
string DataLayoutString(DataLayout layout);

// Specifies a quantization for activations in a given BatchDescriptor.
enum class QuantizedActivationMode {
  k8Bit = 1,
  k16Bit = 2,
  k32Bit = 4,
};

// Describes the dimensions that a layer consumes/produces.
//
// This is a matrix (height, width), its "depth" (feature_map_count),
// how many of these matrices are present (count),
// and the maximum and minimum values expected in the matrix (value_max,
// value_min).
// If input is quantized, all values greater
// than value_max will be clipped to value_max and all values less than
// value_min will be clipped to value_min.
// When quantized output is dequantized no value will be greater than
// value_max or less than value_min.
//
// Uses the named argument construction form:
//
//  auto input_batch_dimensions =
//      BatchDescriptor().set_count(42).set_feature_map_count(7)...
//
// Details:
//
// For a convolutional layer, a single inference takes a 3-dimensional matrix
// of input and produces a 3-dimensional matrix of output. We call the three
// dimensions height, width and feature_map_count, where for an image, the
// height and width correspond to the Y and X pixel indices, respectively, and
// the feature_map_count corresponds to the RGB dimension of the input data.
// Then the count indicates how many 3D matrices are being presented to be
// processed at once; this corresponds to the neural network concept of
// minibatch size.
//
// For a fully connected layer, it's better to put the nodes of the layer in
// the feature_map_count, and leave the height and weight as degenerate (== 1).
// Count indicates how many input vectors (degenerate 3D matrices) are to be
// processed.
//
// If unspecified, value_max and value_min default to 0.0.
// If value_max == value_min the Stream will attempt to derive valid values -
// for example the output of Relu6 activation will always be in the range
// [0.0, 6.0].
//
// If unspecified, layout defaults to kYXDepthBatch.
class BatchDescriptor {
 public:
  // Creates a "blank" batch descriptor, which should be initialized via the
  // named argument helpers.
  BatchDescriptor();

  // Clones values from 'other' for initialization.
  void CloneFrom(const BatchDescriptor& other);

  string ToString() const;
  string ToShortString() const;

  // Accessors.
  int64 count() const { return count_; }
  int64 feature_map_count() const { return feature_map_count_; }
  int64 height() const { return height_; }
  int64 width() const { return width_; }
  float value_max() const { return value_max_; }
  float value_min() const { return value_min_; }
  DataLayout layout() const { return layout_; }
  QuantizedActivationMode quantized_activation_mode() const {
    return quantized_activation_mode_;
  }

  // Named-argument helpers for avoiding user error during construction.
  BatchDescriptor& set_count(int64 value) {
    count_ = value;
    return *this;
  }
  BatchDescriptor& set_feature_map_count(int64 value) {
    feature_map_count_ = value;
    return *this;
  }
  BatchDescriptor& set_height(int64 value) {
    height_ = value;
    return *this;
  }
  BatchDescriptor& set_width(int64 value) {
    width_ = value;
    return *this;
  }
  BatchDescriptor& set_value_max(float value) {
    value_max_ = value;
    return *this;
  }
  BatchDescriptor& set_value_min(float value) {
    value_min_ = value;
    return *this;
  }
  BatchDescriptor& set_layout(DataLayout layout) {
    layout_ = layout;
    return *this;
  }
  BatchDescriptor& set_quantized_activation_mode(
      QuantizedActivationMode quantized_activation_mode) {
    quantized_activation_mode_ = quantized_activation_mode;
    return *this;
  }

  // Return the number of nodes in a single feature map.
  int64 NodesPerFeatureMap() const;

  // Return the number of nodes across all feature maps. Note that this is not
  // affected by the batch count.
  int64 NodesAcrossFeatureMaps() const;

  // Returns the number of elements (e.g. RGB pixel values) required to hold a
  // given batch descriptor, given a no-padding assumption. Note that this is
  // affected by the batch count.
  int64 ElementCount() const;

  // Return the number of weights required to fully connect a layer with
  // dimensions given by the 'input' descriptor with a layer with dimensions
  // given by the 'output' descriptor.
  static int64 FullyConnectedWeightCount(const BatchDescriptor& input,
                                         const BatchDescriptor& output);

  // Return the number of biases required to fully connect to an output layer
  // with dimensions given the 'output' descriptor.
  static int64 FullyConnectedBiasCount(const BatchDescriptor& output);

 private:
  int64 count_;
  int64 feature_map_count_;
  int64 height_;
  int64 width_;
  float value_max_;
  float value_min_;
  DataLayout layout_;
  QuantizedActivationMode quantized_activation_mode_;
};

// Describes how a filter is laid out in the memory.
// Specify int64 so there's no padding in FilterDescriptor.
enum class FilterLayout : int64 {
  kOutputInputYX = 0,  // cuDNN's default filter layout, laid out as:
                       // (major) output feature maps >> input feature maps >>
                       // rows >> columns (minor).
  kInputYXOutput,      // Same as dist_belief's default filter layout.
  kYXInputOutput,      // Same as tensorflow's default filter layout.
};

// Returns a string representation of the given filter layout.
string FilterLayoutString(FilterLayout layout);

// Describes a filter for the convolution. This is the "window" from
// height-by-width patches of each of the feature maps in the input layer to the
// cells within the output feature map.
//
// Uses the named argument construction form:
//
//  FilterDescriptor filter_dimensions;
//  filter_dimensions
//    .set_output_feature_map_count(42)
//    .set_input_feature_map_count(7)
//    ...
//
// Arguments:
// - output_feature_map_count: number of feature maps in the output layer.
// - input_feature_map_count: number of feature maps in the input layer (from
//      which the filter patch is taken).
// - input_filter_height: "height" number of neurons used in the sliding window
//      over the input layer.
// - input_filter_width: "width" number of neurons used in the sliding window
//      over the input layer.
//
// Sometimes names like "filter input height" are referred to by synonymous
// terminology, such as "kernel y size".
//
// If unspecified, layout defaults to kOutputInputYX.
class FilterDescriptor {
 public:
  // By default construction, all dimensions are set to zero, so they should all
  // be populated by the user via the named-argument helpers below. (See class
  // comment for details.)
  FilterDescriptor();

  ~FilterDescriptor();

  // Named-argument helpers for avoiding user error during construction.
  FilterDescriptor& set_output_feature_map_count(int64 value) {
    output_feature_map_count_ = value;
    return *this;
  }
  FilterDescriptor& set_input_feature_map_count(int64 value) {
    input_feature_map_count_ = value;
    return *this;
  }
  FilterDescriptor& set_input_filter_height(int64 value) {
    input_filter_height_ = value;
    return *this;
  }
  FilterDescriptor& set_input_filter_width(int64 value) {
    input_filter_width_ = value;
    return *this;
  }
  FilterDescriptor& set_layout(FilterLayout layout) {
    layout_ = layout;
    return *this;
  }

  void CloneFrom(const FilterDescriptor& other);

  string ToString() const;
  string ToShortString() const;

  // Returns the number of weights required as parameters for a convolution
  // using this filter descriptor.
  int64 ComputeWeightCount() const;

  // Returns the number of biases required as parameters for a convolution using
  // this filter descriptor.
  int64 bias_count() const { return output_feature_map_count_; }

  int64 output_feature_map_count() const { return output_feature_map_count_; }
  int64 input_feature_map_count() const { return input_feature_map_count_; }
  int64 input_filter_height() const { return input_filter_height_; }
  int64 input_filter_width() const { return input_filter_width_; }
  FilterLayout layout() const { return layout_; }

 private:
  int64 output_feature_map_count_;
  int64 input_feature_map_count_;
  int64 input_filter_height_;
  int64 input_filter_width_;
  FilterLayout layout_;

  SE_DISALLOW_COPY_AND_ASSIGN(FilterDescriptor);
};

// Describes a convolution.
//
// Uses the named argument construction form:
//
//  ConvolutionDescriptor convolution_dimensions;
//  convolution_dimensions
//    .set_vertical_filter_stride(2)
//    .set_horizontal_filter_stride(2)
//    ...
//
// Arguments:
// - zero_padding_height: padding of the "y dimension" of the input data. Note
//    that this is different from the height of the filter.
// - zero_padding_width: analogouus to the height above, but in the "x
//    dimension".
// - vertical_filter_stride: the convolution slides a 2-dimensional window of
//    filter-height-by-filter-width over the input layer -- the center of that
//    window is moved in the "y dimension" according to this stride value.
// - horizontal_filter_stride: analogous to the vertical stride above, but in
//    the "x dimension".
class ConvolutionDescriptor {
 public:
  // By default construction, there is no zero-padding and the filter stride is
  // 1x1 (centering the filter on every cell in the input layer's
  // width-by-height area).
  ConvolutionDescriptor();
  ~ConvolutionDescriptor();

  string ToString() const;
  string ToShortString() const;

  ConvolutionDescriptor& set_zero_padding_height(int64 value) {
    zero_padding_height_ = value;
    return *this;
  }
  ConvolutionDescriptor& set_zero_padding_width(int64 value) {
    zero_padding_width_ = value;
    return *this;
  }
  ConvolutionDescriptor& set_vertical_filter_stride(int64 value) {
    vertical_filter_stride_ = value;
    return *this;
  }
  ConvolutionDescriptor& set_horizontal_filter_stride(int64 value) {
    horizontal_filter_stride_ = value;
    return *this;
  }

  int64 zero_padding_height() const { return zero_padding_height_; }
  int64 zero_padding_width() const { return zero_padding_width_; }
  int64 vertical_filter_stride() const { return vertical_filter_stride_; }
  int64 horizontal_filter_stride() const { return horizontal_filter_stride_; }

 private:
  int64 zero_padding_height_;
  int64 zero_padding_width_;
  int64 vertical_filter_stride_;
  int64 horizontal_filter_stride_;
  // TODO(leary) cudnn provides these fields, but need to characterize what
  // their effect is -- they may be boolean rather than integral.
  // int64 upscale_input_x;
  // int64 upscale_input_y;
};

// A patch of values in the input can be pooled via either a max or an average
// operation.
// Specify int64 so there's no padding in PoolingDescriptor.
enum class PoolingMode : int64 {
  kMaximum,
  kAverage,
};

// Describes a pooling operation to be enqueued onto a stream via a platform's
// DnnSupport.
//
// TODO(broune): describe how padding works and what happens if the
// window height/width is not divisible by the vertical/horizontal
// stride.
//
// Arguments:
//  pooling_mode: pooling operator to use on the input patch
//  window_height: height of input window
//  window_width: width of input window
//  vertical_stride: vertical delta for center of the input patch
//  horizontal_stride: horizontal delta for center of the input patch
class PoolingDescriptor {
 public:
  PoolingDescriptor();

  PoolingDescriptor& set_pooling_mode(PoolingMode value) {
    mode_ = value;
    return *this;
  }
  PoolingDescriptor& set_window_height(int64 value) {
    window_height_ = value;
    return *this;
  }
  PoolingDescriptor& set_window_width(int64 value) {
    window_width_ = value;
    return *this;
  }
  PoolingDescriptor& set_vertical_padding(int64 value) {
    vertical_padding_ = value;
    return *this;
  }
  PoolingDescriptor& set_horizontal_padding(int64 value) {
    horizontal_padding_ = value;
    return *this;
  }
  PoolingDescriptor& set_vertical_stride(int64 value) {
    vertical_stride_ = value;
    return *this;
  }
  PoolingDescriptor& set_horizontal_stride(int64 value) {
    horizontal_stride_ = value;
    return *this;
  }

  void CloneFrom(const PoolingDescriptor& other);

  string ToString() const;
  string ToShortString() const;

  PoolingMode mode() const { return mode_; }
  int64 window_height() const { return window_height_; }
  int64 window_width() const { return window_width_; }
  int64 vertical_padding() const { return vertical_padding_; }
  int64 horizontal_padding() const { return horizontal_padding_; }
  int64 vertical_stride() const { return vertical_stride_; }
  int64 horizontal_stride() const { return horizontal_stride_; }

 private:
  PoolingMode mode_;
  int64 window_height_;
  int64 window_width_;
  int64 vertical_padding_;
  int64 horizontal_padding_;
  int64 vertical_stride_;
  int64 horizontal_stride_;

  SE_DISALLOW_COPY_AND_ASSIGN(PoolingDescriptor);
};

// Describes a dist_belief local response normalization.
// The normalization equation is:
// y_i = x_i / (bias + alpha * (sum_j_{i - range}^{i + range} x_j^2)) ^ beta
// where x_i is the input in feature map i, y_i is the output.
// Each feature map is split into segment_size segments for performing the
// sum_j_. If wrap_around is true, the sum_j_ for y_i on the left and right of
// a segment wrap around at the edges of the segment, if wrap_around is false
// zeros are inserted instead.
class NormalizeDescriptor {
 public:
  NormalizeDescriptor();

  NormalizeDescriptor& set_bias(float bias) {
    bias_ = bias;
    return *this;
  }

  NormalizeDescriptor& set_range(int32 range) {
    range_ = range;
    return *this;
  }

  NormalizeDescriptor& set_alpha(float alpha) {
    alpha_ = alpha;
    return *this;
  }

  NormalizeDescriptor& set_beta(float beta) {
    beta_ = beta;
    return *this;
  }

  NormalizeDescriptor& set_wrap_around(bool wrap_around) {
    wrap_around_ = wrap_around;
    return *this;
  }

  NormalizeDescriptor& set_segment_size(int32 segment_size) {
    segment_size_ = segment_size;
    return *this;
  }

  void CloneFrom(const NormalizeDescriptor& other);

  string ToString() const;
  string ToShortString() const;

  float bias() const { return bias_; }
  int32 range() const { return range_; }
  float alpha() const { return alpha_; }
  float beta() const { return beta_; }
  bool wrap_around() const { return wrap_around_; }
  int32 segment_size() const { return segment_size_; }

 private:
  float bias_;
  int32 range_;
  float alpha_;
  float beta_;
  bool wrap_around_;
  int32 segment_size_;

  SE_DISALLOW_COPY_AND_ASSIGN(NormalizeDescriptor);
};

// Describes a kind of non-linearity (threshold-like mathematical function).
enum class ActivationMode {
  kSigmoid,
  // Rectified linear activation: f(x) = x < 0 ? 0 : x
  kRelu,
  // Rectified linear activation, where upper maximum is 6.0.
  kRelu6,
  // Rectified linear activation, where upper maximum specified by
  // BatchDescriptor::value_max().
  kReluX,
  kTanh,
};

// Returns a string representation of the given activation mode.
string ActivationModeString(ActivationMode mode);

// Describes the operation that DoElementwiseOperation should perform on its
// inputs.
enum class ElementwiseOperation {
  kAdd,
  kMultiply
};

string ElementwiseOperationString(ElementwiseOperation op);

// Suite of operations typically used for implementing Deep/Convolutional Neural
// Nets.
class DnnSupport {
 public:
  DnnSupport() {}
  virtual ~DnnSupport() {}

  virtual port::Status Init() = 0;

  // Enqueues a single-precision convolution operation onto the stream.
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'convolve' operation
  //    should be enqueued onto.
  //  input_descriptor: dimensions of the input layer.
  //  input_data: un-owned device memory region which contains the
  //    convolution input.
  //  filter_descriptor: dimensions of the convolution filter.
  //  weights: coefficients for the convolution filter, these are multiplied
  //    against values in the input that the filter convolves over.
  //  convolution_descriptor: stride of the convolution filter.
  //  output_descriptor: dimensions of the output layer.
  //  output_data: un-owned device memory region in which to place the
  //    convolution result.
  //
  // input_descriptor, filter_descriptor, convolution_descriptor and
  // output_descriptor together specify exactly how the convolution is aligned
  // with the input data:
  //
  // * (input dimensions - filter size + 1) / filter stride == output dimensions
  //   corresponds to dist_belief padding = VALID, i.e. the input is not padded.
  // * input dimensions / filter stride == output dimensions
  //   corresponds to dist_belief padding = SAME, i.e. input and output are the
  //   same size - this requires padding the input.
  // * (input dimensions + filter size - 1) / filter stride == output dimensions
  //   corresponds to dist_belief padding = FULL, i.e. the output is sized so
  //   that if the inverse of the filter is applied to the output in VALID mode
  //   the result is the same size as the input - this requires even more
  //   padding
  //   of the input.
  virtual bool DoConvolve(
      Stream* stream, const dnn::BatchDescriptor& input_descriptor,
      const DeviceMemory<float>& input_data,
      const dnn::FilterDescriptor& filter_descriptor,
      const DeviceMemory<float>& filter_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemory<float>* output_data) = 0;

  // Enqueues a double-precision convolution operation onto the stream.
  // See DoConvolve above for argument details.
  virtual bool DoConvolve(
      Stream* stream, const dnn::BatchDescriptor& batch_descriptor,
      const DeviceMemory<double>& input_data,
      const dnn::FilterDescriptor& filter_descriptor,
      const DeviceMemory<double>& filter_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemory<double>* output_data) = 0;

  // Variation of the above with the weight matrix split into two matrices.
  // first_weights: Coefficients of the first matrix.
  // second_weights: Coefficients of the second matrix.
  // depth_multiplier: specifies the columns of the first matrix and rows
  // of the second one - first_weights columns = depth_multiplier,
  // second_weights rows = depth_multiplier *
  //                       filter_descriptor.input_feature_map_count().
  // see go/separable for documentation on separable convolutions.
  virtual bool DoSeparableConvolve(
      Stream* stream, const BatchDescriptor& input_descriptor,
      const DeviceMemory<float>& input_data,
      const FilterDescriptor& filter_descriptor, int depth_multiplier,
      const DeviceMemory<float>& first_weights,
      const DeviceMemory<float>& second_weights,
      const ConvolutionDescriptor& convolution_descriptor,
      const BatchDescriptor& output_descriptor,
      DeviceMemory<float>* output_data) = 0;

  // Enqueues a single-precision backward convolution (for data) operation onto
  // the stream.
  //
  // Arguments:
  //  stream: borrowed pointer to the stream that the 'convolve' operation
  //    should be enqueued onto.
  //  filter_descriptor: dimensions of the convolution filter.
  //  filter_data: coefficients for the convolution filter.
  //  output_descriptor: dimensions of the output gradients, which is the same
  //  as
  //  the dimensions of the ouput.
  //  backward_output_data: un-owned device memory region which contains the
  //    backprop of the output.
  //  convolution_descriptor: stride of the convolution filter.
  //  input_descriptor: dimensions of the input layer.
  //  backward_input_data: un-owned device memory region in which to place the
  //    backprop of the input.
  virtual bool DoConvolveBackwardData(
      Stream* stream, const FilterDescriptor& filter_descriptor,
      const DeviceMemory<float>& filter_data,
      const BatchDescriptor& output_descriptor,
      DeviceMemory<float> backward_output_data,
      const ConvolutionDescriptor& convolution_descriptor,
      const BatchDescriptor& input_descriptor,
      DeviceMemory<float>* backward_input_data) = 0;

  // Enqueues a single-precision backward convolution (for filter) operation
  // onto
  // the stream.
  //
  // Arguments:
  //  stream: borrowed pointer to the stream that the 'convolve' operation
  //    should be enqueued onto.
  //  input_descriptor: dimensions of the input layer.
  //  input_data: un-owned device memory region which contains the
  //    convolution input.
  //  output_descriptor: dimensions of the output gradients, which is the same
  //  as
  //  the dimensions of the ouput.
  //  backward_output_data: un-owned device memory region which contains the
  //    backprop of the output.
  //  convolution_descriptor: stride of the convolution filter.
  //  filter_descriptor: dimensions of the convolution filter.
  //  backward_filter_data: un-owned device memory region in which to place the
  //    backprop of the filter.
  virtual bool DoConvolveBackwardFilter(
      Stream* stream, const BatchDescriptor& input_descriptor,
      const DeviceMemory<float>& input_data,
      const BatchDescriptor& output_descriptor,
      DeviceMemory<float> backward_output_data,
      const ConvolutionDescriptor& convolution_descriptor,
      const FilterDescriptor& filter_descriptor,
      DeviceMemory<float>* backward_filter_data) = 0;

  // Fully connects the "nodes" (float values) in input_data with
  // shape input_dimensions to output_data with output_dimensions
  // using provided weights. This is equivalent to computing a matrix
  // product, hence the name MatMul.
  //
  // A BatchDescriptor has four dimensions: batch, y, x, depth. Matrix products
  // happen in two dimensions. To get down to two dimensions, we consider the
  // input y, x and depth dimension as one combined dimension T. For now,
  // assume that the output height and width are 1 and let OD be the output
  // depth.
  //
  // There are three device memory buffers passed in to this
  // function. We can now view all three as matrices:
  //
  //   input_data: A batch x T matrix
  //   weights: A T x OD matrix
  //   output_data: A batch x OD matrix
  //
  // This function then computes the matrix product of input_data and
  // weights and writes the result into output_data.
  //
  // Here the weights buffer is in row major order, i.e. the first OD
  // entries in weights are the first row, the second OD entries in
  // weights are the second row and so on.
  //
  // The case for output width*height > 1 is more complicated. Let K =
  // OY * OX where OY is the output height and OX is the output
  // width. Then weights is divided into K sub-arrays W_i, for
  // i=0,...,k-1, that each represent a T x OD matrix. This function
  // then computes the K matrix multiplications of input_data with
  // each W_i. This creates K matrices with dimensions batch x
  // OD. These K matrices are concatenated horizontally to form one
  // larger matrix with dimensions batch x (K*OD); note that this is
  // not the same as concatenating the bytes of the matrices. The
  // combined matrix can then be interpreted as a tensor with
  // dimensions (batch, OY, OX, OD). If the output tensor format is
  // not kBatchYXDepth, this function would then need to arrange for
  // the output to be in the requested layout, if that is
  // supported. Note that the case K=1 is equivalent to the
  // description above. It is recommended to prefer the case K=1.
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'fully connect' operation
  //    should be enqueued onto.
  //  output_data: un-owned device memory region in which to place the
  //    fully connected result.
  virtual bool DoMatMul(Stream* stream, const DeviceMemory<float>& input_data,
                        const DeviceMemory<float>& weights,
                        const dnn::BatchDescriptor& input_dimensions,
                        const dnn::BatchDescriptor& output_dimensions,
                        DeviceMemory<float>* output_data) = 0;

  // Version of DoMatMul that uses pre-quantized 8 bit weights.
  // weight_scales specifies the scaling of each column of weights:
  // original float weight[row * num_columns + column] =
  //     quantized_weight[row * nnum_columns + column] * weight_scales[column].
  virtual bool DoMatMulQuantized(Stream* stream,
                                 const DeviceMemory<float>& input_data,
                                 const DeviceMemory<int8>& quantized_weights,
                                 const DeviceMemory<float>& weight_scales,
                                 const dnn::BatchDescriptor& input_dimensions,
                                 const dnn::BatchDescriptor& output_dimensions,
                                 DeviceMemory<float>* output_data) = 0;

  // Version of DoMatMul that uses pre-quantized 16 bit weights.
  // weight_scales specifies the scaling of each column of weights:
  // original float weight[row * num_columns + column] =
  //     quantized_weight[row * nnum_columns + column] * weight_scales[column].
  virtual bool DoMatMulQuantized(Stream* stream,
                                 const DeviceMemory<float>& input_data,
                                 const DeviceMemory<int16>& quantized_weights,
                                 const DeviceMemory<float>& weight_scales,
                                 const dnn::BatchDescriptor& input_dimensions,
                                 const dnn::BatchDescriptor& output_dimensions,
                                 DeviceMemory<float>* output_data) = 0;

  // Adds biases to the feature maps in input_data producing
  // output_data. input_data can equal output_data, but must not
  // partially overlap it.
  //
  // Let K = count() * height() * width() and N = feature_map_count()
  // on dimensions. Then input_value contains K*N values and biases
  // contains N values. We can thus logically consider input_value to
  // contain K vectors of N elements each. This function adds biases
  // to each of those N vectors.
  //
  // TODO(broune): This works differently when width() * height() > 1
  // and the call to ThenBiasAdd() follows a call to ThenMatMul(). In
  // that case there should be width() * height() *
  // feature_map_count() biases, but this is not implemented on all
  // StreamExecutors.
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'bias add' operation
  //    should be enqueued onto.
  //  input_data: un-owned device memory region containing the input.
  //  biases: un-owned device memory region containing biases to add to the
  //    input.
  //  dimensions: dimensions of input_data and output_data.
  //  output_data: un-owned device memory region in which to place the result.
  virtual bool DoBiasAdd(Stream* stream, const DeviceMemory<float>& input_data,
                         const DeviceMemory<float>& biases,
                         const dnn::BatchDescriptor& dimensions,
                         DeviceMemory<float>* output_data) = 0;

  // Performs a forward pooling operation on input_data, writing to
  // output_data. See PoolingDescriptor for how to configure the
  // pooling operation.
  //
  // Pooling happens as a window that moves across the Y and X
  // dimensions of input_data, where each position of the window
  // yields one output value. E.g. for max pooling, the computed value
  // is the maximum element in the window. The operation is applied
  // independently to each batch and at each feature map (depth), so
  // that the output depth and feature_map_count are the same as for
  // the input. The output width and height can be different.
  //
  // See PoolingDescriptor for how to configure the pooling operation.
  virtual bool DoPoolForward(Stream* stream,
                             const dnn::PoolingDescriptor& pooling_dimensions,
                             const dnn::BatchDescriptor& input_dimensions,
                             const DeviceMemory<float>& input_data,
                             const dnn::BatchDescriptor& output_dimensions,
                             DeviceMemory<float>* output_data) = 0;

  // Performs differentiation of the pooling operation.
  virtual bool DoPoolBackward(Stream* stream,
                              const dnn::PoolingDescriptor& pooling_dimensions,
                              const dnn::BatchDescriptor& input_dimensions,
                              const DeviceMemory<float>& input_data,
                              const dnn::BatchDescriptor& output_dimensions,
                              const DeviceMemory<float>& output_data,
                              const DeviceMemory<float>& input_diff_data,
                              DeviceMemory<float>* output_diff_data) = 0;

  // Applies local response normalization to all of the values
  // held on the device in 'input_data'.
  virtual bool DoNormalize(Stream* stream,
                           const dnn::NormalizeDescriptor& normalize_descriptor,
                           const DeviceMemory<float>& input_data,
                           DeviceMemory<float>* output_data) = 0;

  // Applies an activation function (see ActivationMode) to all of the values
  // held on the device in 'input_data', whose dimensions are described by
  // 'dimensions'.
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'activate' operation
  //    should be enqueued onto.
  //  activation_mode: Type of activation to perform.
  //  input_data: un-owned device memory region which contains the
  //    activate input.
  //  output_data: un-owned device memory region in which to place the
  //    activate result.
  virtual bool DoActivate(Stream* stream, ActivationMode activation_mode,
                          const BatchDescriptor& dimensions,
                          const DeviceMemory<float>& input_data,
                          DeviceMemory<float>* output_data) = 0;

  // Concatenates several layers into one, by concatenating the depth of each
  // layer at matching x and y coordinates.
  // The inputs must all have the same width and height, the output will have
  // the same width and height as the inputs and its depth will be the sum of
  // the input depths.
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'depth concatenate'
  // operation should be enqueued onto.
  //  input_dimensions: The dimensions of each input.
  //  input_data: un-owned device memory region which contains the
  //    input data for each input layer.
  //  output_data: un-owned device memory region in which to place the
  //    depth concatenate result.
  virtual bool DoDepthConcatenate(
      Stream* stream, port::ArraySlice<dnn::BatchDescriptor> input_dimensions,
      port::ArraySlice<const DeviceMemory<float>*> input_data,
      DeviceMemory<float>* output_data) = 0;

  // Computes the specified operation (e.g. addition or multiplication)
  // between corresponding elements in the inputs and stores the result in the
  // output element.
  // The inputs and output must all have the same dimensions, but may have
  // different quantization parameters (min_value and max_value).
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'elementwise operation'
  // should be enqueued onto.
  //  operation: The operation to perform.
  //  input_dimensions: The dimensions of each input.
  //  input_data: un-owned device memory region which contains the
  //    input data for each input layer.
  //  output_dimensions: The dimensions of the output.
  //  output_data: un-owned device memory region in which to place the
  //    operation result.
  virtual bool DoElementwiseOperate(
      Stream* stream, ElementwiseOperation operation,
      port::ArraySlice<dnn::BatchDescriptor> input_dimensions,
      port::ArraySlice<const DeviceMemory<float>*> input_data,
      const dnn::BatchDescriptor& output_dimensions,
      DeviceMemory<float>* output_data) = 0;

  // Enqueues an asynchronous memcpy of the *quantized* output of a layer (that
  // is, bytes instead of scaled floats) into 'host_dst' if they are available
  // for the underlying DNN implementation. If this quantized output is not
  // available, false is returned, which will place 'stream' into an error
  // state.
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'quantized memcpy'
  //    operation should be enqueued onto.
  //  gpu_unquantized_src: the device memory that contains the unquantized data
  //    -- this data should also have a corresponding quantized representation
  //    on the device for this operation to succeed.
  //  host_dst: un-owned host memory region that is mutated in place,
  //    it is clobbered by the values in 'gpu_unquantized_src' when the enqueued
  //    (asynchronous) memcpy operation is performed.
  // TODO(wgulland) Merge all these versions of DoMemcpyD2HQuantized.
  virtual bool DoMemcpyD2HQuantized(
      Stream* stream, const DeviceMemory<float>& gpu_unquantized_src,
      port::MutableArraySlice<uint8> host_dst) = 0;

  // As above, but for 16-bit values.
  virtual bool DoMemcpyD2HQuantized(
      Stream* stream, const DeviceMemory<float>& gpu_unquantized_src,
      port::MutableArraySlice<uint16> host_dst) = 0;

  // As above, but for signed 32-bit values.
  virtual bool DoMemcpyD2HQuantized(
      Stream* stream, const DeviceMemory<float>& gpu_unquantized_src,
      port::MutableArraySlice<int32> host_dst) = 0;

  // Enqueues an asynchronous memcpy of 'host_dst' into the *quantized* input
  // of a layer (that is, bytes instead of scaled floats) if they are supported
  // by the underlying DNN implementation. If this quantized input is not
  // supported, false is returned, which will place 'stream' into an error
  // state.
  //
  // Arguments (all borrowed):
  //  stream: borrowed pointer to the stream that the 'quantized memcpy'
  //    operation should be enqueued onto.
  //  host_src: un-owned host memory region that contains the quantized data.
  //  gpu_unquantized_dst: the device memory that is clobbered by the values in
  //    'host_src' when the enqueued (asynchronous) memcpy operation is
  //    performed. -- this data should also have a corresponding quantized
  //    representation on the device for this operation to
  //    succeed.
  virtual bool DoMemcpyH2DQuantized(
      Stream* stream, port::ArraySlice<uint8> host_src,
      DeviceMemory<float>* gpu_unquantized_dst) = 0;

 private:
  SE_DISALLOW_COPY_AND_ASSIGN(DnnSupport);
};

}  // namespace dnn
}  // namespace gputools
}  // namespace perftools

#endif  // TENSORFLOW_STREAM_EXECUTOR_DNN_H_
