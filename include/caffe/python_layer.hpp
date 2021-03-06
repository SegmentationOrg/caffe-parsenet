#ifndef CAFFE_PYTHON_LAYER_HPP_
#define CAFFE_PYTHON_LAYER_HPP_

#include <boost/python.hpp>
#include <vector>

#include "caffe/layer.hpp"

namespace bp = boost::python;

namespace caffe {

template <typename Dtype>
class PythonLayer : public Layer<Dtype> {
 public:
  PythonLayer(PyObject* self, const LayerParameter& param)
      : Layer<Dtype>(param), self_(bp::handle<>(bp::borrowed(self))) { }

  virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
    // Disallow PythonLayer in MultiGPU training stage, due to GIL issues
    // Details: https://github.com/BVLC/caffe/issues/2936
    try {
    self_.attr("param_str") = bp::str(
        this->layer_param_.python_param().param_str());
    }catch (bp::error_already_set) {
      PyErr_Print();
      throw;
    }
    try {
    self_.attr("setup")(bottom, top);
    }catch (bp::error_already_set) {
      PyErr_Print();
      throw;
    }
  }
  virtual void Reshape(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
    try {
    self_.attr("reshape")(bottom, top);
    }catch (bp::error_already_set) {
      PyErr_Print();
      throw;
    }
  }

  virtual inline bool ShareInParallel() const {
    return this->layer_param_.python_param().share_in_parallel();
  }

  virtual inline const char* type() const { return "Python"; }

 protected:
  virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
    try {
    self_.attr("forward")(bottom, top);
    }catch (bp::error_already_set) {
      PyErr_Print();
      throw;
    }

  }
  virtual void Backward_cpu(const vector<Blob<Dtype>*>& top,
      const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom) {
    try {
    self_.attr("backward")(top, propagate_down, bottom);
    }catch (bp::error_already_set) {
      PyErr_Print();
      throw;
    }
  }

 private:
  bp::object self_;
};

}  // namespace caffe

#endif
