#include <iostream>

#include "blaze/graph/graph.h"
#include "blaze/graph/net.h"
#include "blaze/graph/workspace.h"
#include "blaze/model_importer/onnx_importer.h"

std::string to_string(const std::string& str) { return str; }
std::string to_string(const int64_t& v) { return std::to_string(v); }
template <typename T>
std::string to_string(const std::vector<T>& vs) {
  std::stringstream ss;
  ss << "vec[";
  for (auto& v : vs) {
    ss << to_string(v) << ",";
  }
  ss << "]";
  return ss.str();
}

int main() {
  try {
    LOG_SET_LEVEL(blaze::LogLevel::kDebug);
    blaze::ONNXImporter onnx_importer;
    onnx_importer.set_data_type(blaze::kFloat);
    onnx_importer.LoadModel("../../resources/dnn.onnx");
    auto net_def = onnx_importer.net_def();  // copy
    {
      net_def.set_run_mode("simple");
      auto device_option = std::make_unique<blaze::DeviceOption>();
      device_option->set_device_type(0);  // cpu
      net_def.set_allocated_device_option(device_option.release());
    }
    blaze::Workspace workspace;
    workspace.Init(net_def);
    auto net = workspace.CreateNet();
    auto& input_names = net->external_input();
    {
      auto comm_blob = net->external_input_blob("comm");
      comm_blob->Reshape(std::vector<blaze::TIndex>{4,540});
      float* comm_blob_data = comm_blob->as<float>();
      for(auto i = 0; i<comm_blob->size();i++){
        comm_blob_data[i]=1e-4f;
      }
    }
    {
      auto ncomm_blob = net->external_input_blob("ncomm");
      ncomm_blob->Reshape(std::vector<blaze::TIndex>{4,360});
      float* ncomm_blob_data = ncomm_blob->as<float>();
      for(auto i = 0; i<ncomm_blob->size();i++){
        ncomm_blob_data[i]=1e-4f;
      }
    }
    net->Run();
    {
      auto out_blob = net->external_output_blob("out");
      std::cout<<to_string(out_blob->shape())<<"\n";
      float* out_blob_data = out_blob->as<float>();
      std::vector<float> out;
      for(auto i = 0; i<out_blob->size();i++){
        out.push_back(out_blob_data[i]);
      }
      std::cout<<to_string(out)<<"\n";
    }
  } catch (blaze::Exception& e) {
    std::cout << e.what() << "\n";
  }
  return 0;
}