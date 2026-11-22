#ifndef PCA_VISUALIZER_HPP
#define PCA_VISUALIZER_HPP

#include <string>

#include "PCA.hpp"

class PCAVisualizer {
 public:
  PCAVisualizer(PCA& pca, const Matrix& original,
                const std::string& featureX = "Feature X",
                const std::string& featureY = "Feature Y");
  void Run();

 private:
  PCA& pca;
  Matrix original;
  Matrix projected1;
  Matrix projected2;
  Matrix reconstructed1;
  Matrix reconstructed2;

  std::string filename = "example_dataset.csv";
  std::string featureX;
  std::string featureY;
  std::string csvPath;
  std::string loadMessage;
  bool loadError = false;
  bool pathActive = false;

  int step = 0;
  size_t componentCount = 1;
  float rightScroll = 0.0f;

  void UpdateProjectionCache();
  bool LoadCsv(const std::string& path);
  void ResetExampleDataset();
};

#endif
