#include <iostream>

#include "blaze/graph/graph.h"
#include "blaze/model_importer/onnx_importer.h"

int main() {
  blaze::ONNXImporter onnx_importer;
  onnx_importer.set_data_type(blaze::kFloat);
  onnx_importer.LoadModel("../../resources/dnn.onnx");
  auto net_def = onnx_importer.net_def();
  auto graph = blaze::Graph{net_def};
  std::set<std::string> vizgraph_attr_lines;
  std::vector<std::string> vizgraph_topo_lines;
  graph.BFS(
      [&](blaze::Node& node, void* arg) {
        std::string node_name = node.op.name() + "__" + node.op.type();
        for (auto& [index, input_edges] : node.parents) {
          for (auto& input_edge : input_edges) {
            vizgraph_attr_lines.insert(node_name + "[shape=square]");
            vizgraph_topo_lines.push_back(input_edge + "->" + node_name);
          }
        }
        for (auto& [index, output_edges] : node.children) {
          for (auto& output_edge : output_edges) {
            vizgraph_topo_lines.push_back(node_name + "->" + output_edge);
          }
        }
        return false;
      },
      nullptr);
  {
    std::cout << "digraph G {\n";
    for (auto& line : vizgraph_attr_lines) {
      std::cout << "    " << line << '\n';
    }
    for (auto& line : vizgraph_topo_lines) {
      std::cout << "    " << line << '\n';
    }
    std::cout << "}";
  }
  return 0;
}