#include "ui/PCAVisualizer.hpp"

#include "ui/CsvLoader.hpp"
#include "raylib_ui.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct PlotArea {
  float x;
  float y;
  float width;
  float height;
};

struct PlotBounds {
  double minX;
  double maxX;
  double minY;
  double maxY;
};

const char* StepTitles[] = {
  "Original Data", "Centering", "Covariance", "Eigenvectors",
  "Projection", "Reconstruction"
};

Rectangle Rect(float x, float y, float w, float h) {
  return {x, y, w, h};
}

bool Button(Rectangle rect, const char* text, int fontSize = 16,
            Color base = PANEL, Color textColor = TEXT) {
  Vector2 mouse = GetMousePosition();
  bool hover = CheckCollisionPointRec(mouse, rect);
  Color color = hover ? INDIGO : base;
  DrawRectangleRounded(rect, 0.16f, 8, color);
  DrawRectangleRoundedLines(rect, 0.16f, 8, BORDER);
  int width = MeasureText(text, fontSize);
  DrawText(text, static_cast<int>(rect.x + (rect.width - width) * 0.5f),
           static_cast<int>(rect.y + (rect.height - fontSize) * 0.5f),
           fontSize, textColor);
  return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

std::string Format(double value, int precision = 3) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(precision) << value;
  return out.str();
}

std::vector<std::string> WrapText(const char* text, int fontSize,
                                  int maxWidth) {
  std::vector<std::string> lines;
  std::istringstream words(text);
  std::string word;
  std::string line;
  while (words >> word) {
    std::string candidate = line.empty() ? word : line + " " + word;
    if (MeasureText(candidate.c_str(), fontSize) <= maxWidth) {
      line = candidate;
    } else {
      if (!line.empty()) lines.push_back(line);
      line = word;
    }
  }
  if (!line.empty()) lines.push_back(line);
  return lines;
}

std::string Trim(const std::string& text) {
  size_t first = text.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) return "";
  size_t last = text.find_last_not_of(" \t\r\n");
  return text.substr(first, last - first + 1);
}

void DrawWrappedText(const char* text, int x, int y, int width,
                     int fontSize, Color color, int lineHeight) {
  for (const std::string& line : WrapText(text, fontSize, width)) {
    DrawText(line.c_str(), x, y, fontSize, color);
    y += lineHeight;
  }
}

PlotBounds ExpandBounds(double minX, double maxX, double minY, double maxY) {
  double rangeX = std::max(maxX - minX, 1.0);
  double rangeY = std::max(maxY - minY, 1.0);
  return {minX - rangeX * 0.10, maxX + rangeX * 0.10,
          minY - rangeY * 0.10, maxY + rangeY * 0.10};
}

Vector2 MapPoint(double x, double y, const PlotArea& plot,
                 const PlotBounds& bounds) {
  double worldWidth = std::max(bounds.maxX - bounds.minX, 1e-9);
  double worldHeight = std::max(bounds.maxY - bounds.minY, 1e-9);
  double scale = std::min(plot.width / worldWidth, plot.height / worldHeight);
  double midX = (bounds.minX + bounds.maxX) * 0.5;
  double midY = (bounds.minY + bounds.maxY) * 0.5;
  return {static_cast<float>(plot.x + plot.width * 0.5 + (x - midX) * scale),
          static_cast<float>(plot.y + plot.height * 0.5 - (y - midY) * scale)};
}

Matrix MakeMatrix(const std::vector<std::vector<double>>& rows) {
  Matrix matrix(static_cast<int>(rows.size()), 2);
  for (size_t i = 0; i < rows.size(); ++i) {
    matrix(i, 0) = rows[i][0];
    matrix(i, 1) = rows[i][1];
  }
  return matrix;
}

}  // namespace

PCAVisualizer::PCAVisualizer(PCA& pcaRef, const Matrix& originalData,
                             const std::string& xName,
                             const std::string& yName)
    : pca(pcaRef), original(originalData),
      projected1(pcaRef.Transform(originalData, 1)),
      projected2(pcaRef.Transform(originalData, 2)),
      reconstructed1(pcaRef.InverseTransform(projected1)),
      reconstructed2(pcaRef.InverseTransform(projected2)),
      featureX(xName), featureY(yName) {}

void PCAVisualizer::UpdateProjectionCache() {
  projected1 = pca.Transform(original, 1);
  projected2 = pca.Transform(original, 2);
  reconstructed1 = pca.InverseTransform(projected1);
  reconstructed2 = pca.InverseTransform(projected2);
}

bool PCAVisualizer::LoadCsv(const std::string& path) {
  CsvLoadResult result = LoadTwoColumnCsv(path);
  if (!result.success) {
    loadError = true;
    loadMessage = result.error;
    return false;
  }

  Matrix candidateData = MakeMatrix(result.rows);
  PCA candidate;
  try {
    candidate.Fit(candidateData);
  } catch (const std::exception& error) {
    loadError = true;
    loadMessage = error.what();
    return false;
  }

  pca = candidate;
  original = candidateData;
  featureX = result.firstFeature;
  featureY = result.secondFeature;
  filename = result.filename;
  step = 0;
  rightScroll = 0.0f;
  UpdateProjectionCache();
  loadError = false;
  loadMessage = "Loaded " + std::to_string(original.Rows()) + " observations.";
  return true;
}

void PCAVisualizer::ResetExampleDataset() {
  Matrix data(6, 2);
  data(0,0)=1.0; data(0,1)=2.0;
  data(1,0)=2.0; data(1,1)=4.0;
  data(2,0)=3.0; data(2,1)=3.0;
  data(3,0)=4.0; data(3,1)=6.0;
  data(4,0)=5.0; data(4,1)=7.0;
  data(5,0)=6.0; data(5,1)=8.0;

  PCA example;
  example.Fit(data);
  pca = example;
  original = data;
  filename = "example_dataset.csv";
  featureX = "Feature X";
  featureY = "Feature Y";
  step = 0;
  rightScroll = 0.0f;
  loadError = false;
  loadMessage = "Example dataset restored.";
  UpdateProjectionCache();
}

void PCAVisualizer::Run() {
  InitWindow(1280, 720, "PCA Visualizer");
  SetWindowMinSize(1100, 650);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BG);

    if (!pathActive) {
      if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE)) {
        step = std::min(5, step + 1);
        rightScroll = 0.0f;
      }
      if (IsKeyPressed(KEY_LEFT)) {
        step = std::max(0, step - 1);
        rightScroll = 0.0f;
      }
    }

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    const int sidebarWidth = 230;
    const int rightWidth = 300;
    const int centerWidth = screenWidth - sidebarWidth - rightWidth;

    DrawRectangleRec(Rect(0, 0, sidebarWidth, screenHeight), SIDEBAR);
    DrawRectangleRec(Rect(sidebarWidth, 0, centerWidth, screenHeight), BG);
    DrawRectangleRec(Rect(screenWidth - rightWidth, 0, rightWidth, screenHeight), SIDEBAR);

    DrawLine(sidebarWidth, 0, sidebarWidth, screenHeight, BORDER);
    DrawLine(screenWidth - rightWidth, 0, screenWidth - rightWidth, screenHeight, BORDER);

    // Sidebar
    DrawText("PCA Visualizer", 20, 22, 20, TEXT);
    DrawText("DATASET", 20, 65, 12, MUTED);
    if (Button(Rect(20, 88, 190, 38), "Load CSV", 16, CYAN, SIDEBAR)) {
      pathActive = true;
      csvPath = "";
    }

    DrawText(filename.substr(0, 24).c_str(), 20, 136, 13, TEXT);
    DrawText((std::to_string(original.Rows()) + " observations").c_str(),
             20, 156, 13, MUTED);

    DrawRectangleRounded(Rect(20, 178, 190, 34), 0.12f, 6, PANEL);
    DrawRectangleRoundedLines(Rect(20, 178, 190, 34), 0.12f, 6,
                             pathActive ? CYAN : BORDER);
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, Rect(20, 178, 190, 34)) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      pathActive = true;
    }
    if (pathActive) {
      int key = GetCharPressed();
      while (key > 0) {
        if (key >= 32 && key <= 125) csvPath.push_back(static_cast<char>(key));
        key = GetCharPressed();
      }
      if (IsKeyPressed(KEY_BACKSPACE) && !csvPath.empty()) csvPath.pop_back();
      if (IsKeyPressed(KEY_ENTER)) {
        LoadCsv(csvPath);
        pathActive = false;
      }
      if (IsKeyPressed(KEY_ESCAPE)) {
        pathActive = false;
      }
    }
    if (csvPath.empty()) {
      DrawText(pathActive ? "Type path..." : "Enter CSV path",
               28, 188, 13, pathActive ? CYAN : MUTED);
    } else {
      DrawText(csvPath.substr(0, 24).c_str(), 28, 188, 13, TEXT);
    }
    if (pathActive) {
      std::string visibleText = csvPath.empty() ? "Type path..." :
                                csvPath.substr(0, 24);
      int caretX = 28 + MeasureText(visibleText.c_str(), 13);
      DrawText("|", caretX, 188, 13, CYAN);
    }
    if (Button(Rect(20, 220, 190, 34), "Browse/Load", 14, PANEL, TEXT)) {
      LoadCsv(csvPath);
      pathActive = false;
    }

    if (Button(Rect(20, 262, 190, 34), "Reset example", 14, PANEL, TEXT)) {
      ResetExampleDataset();
    }

    DrawText("PCA STEPS", 20, 316, 12, MUTED);
    const char* stepNames[] = {"1  Original Data", "2  Centering", "3  Covariance",
                               "4  Eigenvectors", "5  Projection", "6  Reconstruction"};
    for (int i = 0; i < 6; ++i) {
      Rectangle rect = Rect(20, 338.0f + i * 42.0f, 190, 34);
      bool selected = i == step;
      DrawRectangleRounded(rect, 0.14f, 8,
                           selected ? CYAN : (CheckCollisionPointRec(GetMousePosition(), rect) ? PANEL : SIDEBAR));
      DrawRectangleRoundedLines(rect, 0.14f, 8,
                                selected ? CYAN : BORDER);
      DrawText(stepNames[i], 32, static_cast<int>(rect.y + 9), 15,
               selected ? SIDEBAR : TEXT);
      if (CheckCollisionPointRec(GetMousePosition(), rect) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        step = i;
        rightScroll = 0.0f;
      }
    }

    DrawText("COMPONENTS", 20, 610, 12, MUTED);
    if (Button(Rect(20, 632, 90, 34), "1 component", 13,
               componentCount == 1 ? CYAN : PANEL,
               componentCount == 1 ? SIDEBAR : TEXT)) {
      componentCount = 1;
      UpdateProjectionCache();
    }
    if (Button(Rect(120, 632, 90, 34), "2 components", 13,
               componentCount == 2 ? CYAN : PANEL,
               componentCount == 2 ? SIDEBAR : TEXT)) {
      componentCount = 2;
      UpdateProjectionCache();
    }

    // Center plot panel
    const float centerX = sidebarWidth + 16.0f;
    const float centerY = 16.0f;
    const float centerW = centerWidth - 32.0f;
    const float centerH = screenHeight - 32.0f;
    DrawRectangleRounded(Rect(centerX, centerY, centerW, centerH), 0.018f, 8, PANEL);
    DrawRectangleRoundedLines(Rect(centerX, centerY, centerW, centerH), 0.018f, 8, BORDER);

    DrawText(StepTitles[step], static_cast<int>(centerX + 20), 34, 18, TEXT);
    DrawText((featureX + " / " + featureY).c_str(),
             static_cast<int>(centerX + 20), 58, 13, MUTED);
    DrawText("Left / Right or Space",
             static_cast<int>(centerX + centerW - 300), 58, 12, MUTED);

    if (Button(Rect(centerX + centerW - 210.0f, 24.0f, 88.0f, 34.0f),
               "< Prev", 14, PANEL, TEXT) && step > 0) {
      --step;
      rightScroll = 0.0f;
    }
    if (Button(Rect(centerX + centerW - 112.0f, 24.0f, 88.0f, 34.0f),
               "Next >", 14, PANEL, TEXT) && step < 5) {
      ++step;
      rightScroll = 0.0f;
    }

    PlotArea plot{centerX + 58.0f, 108.0f,
                  centerW - 78.0f, centerH - 180.0f};

    auto means = pca.Means();
    auto eigenvectors = pca.Eigenvectors();

    double minX = 1e100, maxX = -1e100, minY = 1e100, maxY = -1e100;
    auto include = [&](double x, double y) {
      minX = std::min(minX, x); maxX = std::max(maxX, x);
      minY = std::min(minY, y); maxY = std::max(maxY, y);
    };

    if (step == 1) {
      for (size_t row = 0; row < original.Rows(); ++row) {
        include(original(row, 0) - means[0], original(row, 1) - means[1]);
      }
      include(0.0, 0.0);
    } else if (step == 5) {
      const Matrix& reconstruction = componentCount == 1 ? reconstructed1 : reconstructed2;
      for (size_t row = 0; row < original.Rows(); ++row) {
        include(original(row, 0), original(row, 1));
        include(reconstruction(row, 0), reconstruction(row, 1));
      }
    } else {
      for (size_t row = 0; row < original.Rows(); ++row) {
        include(original(row, 0), original(row, 1));
      }
      include(means[0], means[1]);
    }

    PlotBounds bounds = ExpandBounds(minX, maxX, minY, maxY);

    BeginScissorMode(static_cast<int>(plot.x), static_cast<int>(plot.y),
                     static_cast<int>(plot.width), static_cast<int>(plot.height));

    for (int i = 0; i <= 8; ++i) {
      float px = plot.x + plot.width * i / 8.0f;
      float py = plot.y + plot.height * i / 8.0f;
      DrawLine(static_cast<int>(px), static_cast<int>(plot.y),
               static_cast<int>(px), static_cast<int>(plot.y + plot.height), BORDER);
      DrawLine(static_cast<int>(plot.x), static_cast<int>(py),
               static_cast<int>(plot.x + plot.width), static_cast<int>(py), BORDER);
    }

    Vector2 zero = step == 1 ? MapPoint(0.0, 0.0, plot, bounds)
                             : MapPoint(0.0, 0.0, plot, bounds);
    if (zero.x >= plot.x && zero.x <= plot.x + plot.width)
      DrawLineEx({zero.x, plot.y}, {zero.x, plot.y + plot.height}, 1.5f, MUTED);
    if (zero.y >= plot.y && zero.y <= plot.y + plot.height)
      DrawLineEx({plot.x, zero.y}, {plot.x + plot.width, zero.y}, 1.5f, MUTED);

    auto mapOriginal = [&](size_t row) {
      return MapPoint(original(row, 0), original(row, 1), plot, bounds);
    };

    if (step == 0 || step == 2) {
      for (size_t row = 0; row < original.Rows(); ++row) {
        DrawCircleV(mapOriginal(row), 5.5f, CYAN);
      }
    } else if (step == 1) {
      Vector2 mean = MapPoint(0.0, 0.0, plot, bounds);
      for (size_t row = 0; row < original.Rows(); ++row) {
        Vector2 p = MapPoint(original(row, 0) - means[0],
                             original(row, 1) - means[1], plot, bounds);
        DrawCircleV(p, 5.5f, BLUE);
      }
      DrawCircleV(mean, 8.0f, AMBER);
    } else if (step == 3) {
      for (size_t row = 0; row < original.Rows(); ++row)
        DrawCircleV(mapOriginal(row), 5.5f, CYAN);
      Vector2 mean = MapPoint(means[0], means[1], plot, bounds);
      DrawCircleV(mean, 8.0f, AMBER);

      double worldSpan = std::max(bounds.maxX - bounds.minX,
                                  bounds.maxY - bounds.minY);
      double arrow = worldSpan * 0.18;
      for (size_t col = 0; col < 2; ++col) {
        Vector2 end = MapPoint(means[0] + eigenvectors(0, col) * arrow,
                               means[1] + eigenvectors(1, col) * arrow,
                               plot, bounds);
        Color color = col == 0 ? GREEN : PURPLE;
        DrawLineEx(mean, end, 4.0f, color);
        Vector2 direction = {end.x - mean.x, end.y - mean.y};
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 8.0f) {
          direction.x /= length; direction.y /= length;
          DrawTriangle(end,
                       {end.x - direction.x * 15.0f - direction.y * 7.0f,
                        end.y - direction.y * 15.0f + direction.x * 7.0f},
                       {end.x - direction.x * 15.0f + direction.y * 7.0f,
                        end.y - direction.y * 15.0f - direction.x * 7.0f}, color);
        }
        DrawText(col == 0 ? "PC1" : "PC2",
                 static_cast<int>(end.x + (col == 0 ? 8 : -32)),
                 static_cast<int>(end.y - 18), 13, color);
      }
    } else if (step == 4) {
      for (size_t row = 0; row < original.Rows(); ++row)
        DrawCircleV(mapOriginal(row), 5.0f, CYAN);

      Vector2 mean = MapPoint(means[0], means[1], plot, bounds);
      DrawCircleV(mean, 8.0f, AMBER);
      double worldSpan = std::max(bounds.maxX - bounds.minX,
                                  bounds.maxY - bounds.minY);
      double arrow = worldSpan * 0.18;
      Vector2 pcEnd = MapPoint(means[0] + eigenvectors(0,0)*arrow,
                               means[1] + eigenvectors(1,0)*arrow,
                               plot, bounds);
      DrawLineEx(mean, pcEnd, 3.0f, GREEN);

      const Matrix& reconstructed = componentCount == 1 ? reconstructed1 : reconstructed2;
      for (size_t row = 0; row < reconstructed.Rows(); ++row) {
        Vector2 p = MapPoint(reconstructed(row,0), reconstructed(row,1), plot, bounds);
        DrawCircleV(p, 5.5f, PURPLE);
      }
    } else if (step == 5) {
      const Matrix& reconstruction = componentCount == 1 ? reconstructed1 : reconstructed2;
      for (size_t row = 0; row < original.Rows(); ++row) {
        Vector2 a = mapOriginal(row);
        Vector2 b = MapPoint(reconstruction(row,0), reconstruction(row,1), plot, bounds);
        DrawLineEx(a, b, 1.2f, ORANGE);
        DrawCircleV(a, 5.5f, CYAN);
        DrawCircleV(b, 6.0f, ORANGE);
      }
    }

    EndScissorMode();

    // Tick labels
    for (int i = 0; i <= 4; ++i) {
      double xValue = bounds.minX + (bounds.maxX - bounds.minX) * i / 4.0;
      double yValue = bounds.minY + (bounds.maxY - bounds.minY) * i / 4.0;
      Vector2 xt = MapPoint(xValue, bounds.minY, plot, bounds);
      Vector2 yt = MapPoint(bounds.minX, yValue, plot, bounds);
      std::string xText = Format(xValue, 1);
      std::string yText = Format(yValue, 1);
      int xLabel = static_cast<int>(xt.x - MeasureText(xText.c_str(), 11) * 0.5f);
      xLabel = std::max(static_cast<int>(plot.x),
                        std::min(xLabel, static_cast<int>(plot.x + plot.width - 35)));
      DrawText(xText.c_str(), xLabel,
               static_cast<int>(plot.y + plot.height + 8), 11, MUTED);
      int yLabel = static_cast<int>(yt.y - 6);
      yLabel = std::max(static_cast<int>(plot.y),
                        std::min(yLabel, static_cast<int>(plot.y + plot.height - 12)));
      DrawText(yText.c_str(), static_cast<int>(plot.x - 48), yLabel, 11, MUTED);
    }

    DrawText(featureX.c_str(), static_cast<int>(plot.x + plot.width - 90),
             static_cast<int>(plot.y + plot.height + 24), 12, MUTED);
    DrawText(featureY.c_str(), static_cast<int>(plot.x + 4),
             static_cast<int>(plot.y - 28), 12, MUTED);

    // Legend
    float legendY = centerY + centerH - 42.0f;
    float legendX = centerX + 20.0f;
    struct LegendItem { Color color; const char* label; };
    std::vector<LegendItem> legend;
    if (step == 0 || step == 2 || step == 3 || step == 4 || step == 5)
      legend.push_back({CYAN, "Original"});
    if (step == 1) legend.push_back({BLUE, "Centered"});
    if (step == 1 || step == 3 || step == 4)
      legend.push_back({AMBER, "Mean"});
    if (step == 3) { legend.push_back({GREEN, "PC1"}); legend.push_back({PURPLE, "PC2"}); }
    if (step == 4) legend.push_back({PURPLE, "Projected"});
    if (step == 5) legend.push_back({ORANGE, "Reconstructed"});
    for (const LegendItem& item : legend) {
      DrawCircleV({legendX + 5, legendY + 6}, 5.0f, item.color);
      DrawText(item.label, static_cast<int>(legendX + 16),
               static_cast<int>(legendY), 12, MUTED);
      legendX += 90.0f;
    }
    // Right explanation panel
    DrawText("CURRENT STEP", screenWidth - rightWidth + 20, 25, 12, MUTED);
    DrawText(StepTitles[step], screenWidth - rightWidth + 20, 52, 19, TEXT);

    rightScroll -= GetMouseWheelMove() * 24.0f;
    rightScroll = std::max(0.0f, std::min(rightScroll, 380.0f));

    Rectangle scrollArea = Rect(static_cast<float>(screenWidth - rightWidth + 20),
                                88.0f, 260.0f, screenHeight - 108.0f);
    BeginScissorMode(static_cast<int>(scrollArea.x), static_cast<int>(scrollArea.y),
                     static_cast<int>(scrollArea.width), static_cast<int>(scrollArea.height));

    int y = static_cast<int>(scrollArea.y - rightScroll);
    const char* explanation = "";
    if (step == 0) explanation = "Each point is one observation described by two numeric features.";
    if (step == 1) explanation = "Centering subtracts each feature's mean, shifting the dataset mean to the origin without changing relative positions.";
    if (step == 2) explanation = "Covariance describes how the two features vary together. Variance measures variation within a feature.";
    if (step == 3) explanation = "PC1 is the direction of greatest variance. PC2 is perpendicular and captures the remaining variance.";
    if (step == 4) explanation = "PCA coordinates combine the learned feature directions. Retaining one component reduces observations to one dimension; retaining two keeps both directions.";
    if (step == 5) explanation = "InverseTransform maps PCA coordinates back into feature space. Discarding PC2 loses detail, while two components approximately recover the original points.";

    auto explanationLines = WrapText(explanation, 15, 260);
    for (const std::string& line : explanationLines) {
      DrawText(line.c_str(), screenWidth - rightWidth + 20, y, 15, MUTED);
      y += 21;
    }
    y += 18;

    DrawText("CALCULATED VALUES", screenWidth - rightWidth + 20, y, 12, CYAN);
    y += 28;

    auto eigenvalues = pca.Eigenvalues();
    if (step == 0) {
      DrawText(("Observations  " + std::to_string(original.Rows())).c_str(),
               screenWidth - rightWidth + 20, y, 15, TEXT); y += 25;
      DrawText("Features  2", screenWidth - rightWidth + 20, y, 15, TEXT); y += 25;
    } else if (step == 1) {
      DrawText("Feature means", screenWidth - rightWidth + 20, y, 15, TEXT); y += 24;
      DrawText(("Mean " + featureX + "  " + Format(means[0])).c_str(),
               screenWidth - rightWidth + 20, y, 14, MUTED); y += 22;
      DrawText(("Mean " + featureY + "  " + Format(means[1])).c_str(),
               screenWidth - rightWidth + 20, y, 14, MUTED); y += 22;
    } else if (step == 2) {
      DrawText("Covariance matrix", screenWidth - rightWidth + 20, y, 15, TEXT); y += 24;
      DrawText("Not exposed by PCA API", screenWidth - rightWidth + 20, y, 14, AMBER); y += 22;
      DrawText("Missing: CovarianceMatrix()", screenWidth - rightWidth + 20, y, 13, MUTED); y += 22;
    } else if (step == 3) {
      DrawText("Eigenvalues", screenWidth - rightWidth + 20, y, 15, TEXT); y += 24;
      DrawText(("PC1  " + Format(eigenvalues[0])).c_str(),
               screenWidth - rightWidth + 20, y, 14, GREEN); y += 22;
      DrawText(("PC2  " + Format(eigenvalues[1])).c_str(),
               screenWidth - rightWidth + 20, y, 14, PURPLE); y += 28;
      DrawText("Eigenvectors (columns)", screenWidth - rightWidth + 20, y, 15, TEXT); y += 24;
      DrawText(("PC1  (" + Format(eigenvectors(0,0)) + ", " +
                Format(eigenvectors(1,0)) + ")").c_str(),
               screenWidth - rightWidth + 20, y, 13, GREEN); y += 22;
      DrawText(("PC2  (" + Format(eigenvectors(0,1)) + ", " +
                Format(eigenvectors(1,1)) + ")").c_str(),
               screenWidth - rightWidth + 20, y, 13, PURPLE); y += 28;
      DrawText("Explained variance ratio", screenWidth - rightWidth + 20, y, 15, TEXT); y += 22;
      DrawText("Not exposed by PCA API", screenWidth - rightWidth + 20, y, 13, AMBER); y += 22;
    } else if (step == 4) {
      DrawText("Projection count", screenWidth - rightWidth + 20, y, 15, TEXT); y += 24;
      DrawText(componentCount == 1 ? "1 principal component" : "2 principal components",
               screenWidth - rightWidth + 20, y, 14, CYAN); y += 28;
      DrawText("First observation PCA coordinates", screenWidth - rightWidth + 20, y, 14, TEXT); y += 24;
      const Matrix& projected = componentCount == 1 ? projected1 : projected2;
      std::string coordinate = Format(projected(0,0));
      if (componentCount == 2) coordinate += ", " + Format(projected(0,1));
      DrawText(("(" + coordinate + ")").c_str(),
               screenWidth - rightWidth + 20, y, 14, MUTED); y += 28;
      DrawText("Method", screenWidth - rightWidth + 20, y, 15, TEXT); y += 22;
      DrawText("PCA::Transform()", screenWidth - rightWidth + 20, y, 13, MUTED);
    } else {
      DrawText("Reconstruction", screenWidth - rightWidth + 20, y, 15, TEXT); y += 24;
      const Matrix& reconstruction = componentCount == 1 ? reconstructed1 : reconstructed2;
      double squaredError = 0.0;
      for (size_t row = 0; row < original.Rows(); ++row) {
        for (size_t col = 0; col < 2; ++col) {
          double difference = original(row,col) - reconstruction(row,col);
          squaredError += difference * difference;
        }
      }
      DrawText(("Squared error  " + Format(squaredError)).c_str(),
               screenWidth - rightWidth + 20, y, 14, AMBER); y += 24;
      DrawText(("First original  (" + Format(original(0,0)) + ", " +
                Format(original(0,1)) + ")").c_str(),
               screenWidth - rightWidth + 20, y, 13, CYAN); y += 22;
      DrawText(("First reconstructed  (" + Format(reconstruction(0,0)) + ", " +
                Format(reconstruction(0,1)) + ")").c_str(),
               screenWidth - rightWidth + 20, y, 13, ORANGE); y += 28;
      DrawText("Method", screenWidth - rightWidth + 20, y, 15, TEXT); y += 22;
      DrawText("PCA::InverseTransform()", screenWidth - rightWidth + 20, y, 13, MUTED);
    }

    if (!loadMessage.empty()) {
      y += 24;
      DrawText(loadError ? "CSV error" : "CSV status",
               screenWidth - rightWidth + 20, y, 13,
               loadError ? RED : GREEN); y += 20;
      DrawWrappedText(loadMessage.c_str(), screenWidth - rightWidth + 20,
                      y, 260, 12, MUTED, 18);
    }

    EndScissorMode();

    EndDrawing();
  }

  CloseWindow();
}
