#include "PCA.hpp"
#include "ui/PCAVisualizer.hpp"

int main() {
  Matrix data{6, 2};

  data(0, 0) = 1.0;  data(0, 1) = 2.0;
  data(1, 0) = 2.0;  data(1, 1) = 4.0;
  data(2, 0) = 3.0;  data(2, 1) = 3.0;
  data(3, 0) = 4.0;  data(3, 1) = 6.0;
  data(4, 0) = 5.0;  data(4, 1) = 7.0;
  data(5, 0) = 6.0;  data(5, 1) = 8.0;

  PCA pca;
  pca.Fit(data);

  PCAVisualizer visualizer(pca, data);
  visualizer.Run();
  return 0;
}