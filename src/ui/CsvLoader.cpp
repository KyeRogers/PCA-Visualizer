#include "ui/CsvLoader.hpp"

#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {

std::string Trim(const std::string& text) {
  size_t first = 0;
  while (first < text.size() &&
         std::isspace(static_cast<unsigned char>(text[first]))) {
    ++first;
  }
  size_t last = text.size();
  while (last > first &&
         std::isspace(static_cast<unsigned char>(text[last - 1]))) {
    --last;
  }
  return text.substr(first, last - first);
}

std::vector<std::string> SplitCsvLine(const std::string& line) {
  std::vector<std::string> fields;
  std::stringstream stream(line);
  std::string field;
  while (std::getline(stream, field, ',')) {
    fields.push_back(Trim(field));
  }
  if (!line.empty() && line.back() == ',') fields.push_back("");
  return fields;
}

bool ParseDouble(const std::string& text, double& value) {
  if (text.empty()) return false;
  std::istringstream stream(text);
  stream >> std::noskipws >> value;
  return stream.eof() && !stream.fail();
}

std::string Filename(const std::string& path) {
  size_t slash = path.find_last_of("/\\");
  return slash == std::string::npos ? path : path.substr(slash + 1);
}

}  // namespace

CsvLoadResult LoadTwoColumnCsv(const std::string& path) {
  CsvLoadResult result;
  result.filename = Filename(path);

  std::ifstream file(path);
  if (!file.is_open()) {
    result.error = "Could not open this file. Check the path and try again.";
    return result;
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    if (!Trim(line).empty()) lines.push_back(line);
  }

  if (lines.empty()) {
    result.error = "The CSV file is empty.";
    return result;
  }

  size_t firstDataLine = 0;
  std::vector<std::string> firstFields = SplitCsvLine(lines[0]);
  bool firstRowIsHeader = false;
  if (firstFields.size() == 2) {
    double ignored;
    firstRowIsHeader = !ParseDouble(firstFields[0], ignored) ||
                       !ParseDouble(firstFields[1], ignored);
  }

  if (firstRowIsHeader) {
    if (!firstFields[0].empty()) result.firstFeature = firstFields[0];
    if (!firstFields[1].empty()) result.secondFeature = firstFields[1];
    firstDataLine = 1;
  }

  if (lines.size() - firstDataLine < 2) {
    result.error = "CSV must contain at least two valid observations.";
    return result;
  }

  for (size_t i = firstDataLine; i < lines.size(); ++i) {
    std::vector<std::string> fields = SplitCsvLine(lines[i]);
    if (fields.size() != 2) {
      result.error = "Line " + std::to_string(i + 1) +
                     " has " + std::to_string(fields.size()) +
                     " columns; exactly 2 are required.";
      return result;
    }

    double x;
    double y;
    if (!ParseDouble(fields[0], x) || !ParseDouble(fields[1], y)) {
      result.error = "Line " + std::to_string(i + 1) +
                     " contains a non-numeric value; malformed rows are not skipped.";
      return result;
    }

    result.rows.push_back({x, y});
  }

  result.success = true;
  return result;
}
