#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "MarkdownWindow.h"
#include "yaml-cpp/node/node.h"

void MarkdownWindow::WriteRecentlyOpenedFilesIfNotExists()
{
  const std::string fileName = "recent_files.yaml";
  std::filesystem::path recentYamlFilePath = this->recentlyOpenedFilesPath.ToStdString();
  recentYamlFilePath /= fileName;
  if(!std::filesystem::exists(recentYamlFilePath))
  {
    YAML::Node root;
    root["max_size"] = this->defaultRecentlyOpenedFilesSize;
    root["current_index"] = 0;
    std::ofstream outputFile(recentYamlFilePath);
    outputFile << root;
    outputFile.close();
  }
}
