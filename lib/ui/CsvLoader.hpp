#ifndef CSV_LOADER_HPP
#define CSV_LOADER_HPP

#include <string>
#include <vector>

struct CsvLoadResult {
  bool success = false;
  std::string error;
  std::string filename;
  std::string firstFeature = "Feature 1";
  std::string secondFeature = "Feature 2";
  std::vector<std::vector<double>> rows;
};

CsvLoadResult LoadTwoColumnCsv(const std::string& path);

#endif
