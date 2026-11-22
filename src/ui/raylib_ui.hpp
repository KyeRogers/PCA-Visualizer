#ifndef RAYLIB_UI_HPP
#define RAYLIB_UI_HPP

struct Vector2 { float x; float y; };
struct Rectangle { float x; float y; float width; float height; };
struct Color { unsigned char r; unsigned char g; unsigned char b; unsigned char a; };

constexpr int MOUSE_LEFT_BUTTON = 0;

inline const Color BG = {11, 18, 32, 255};
inline const Color SIDEBAR = {17, 28, 48, 255};
inline const Color PANEL = {23, 36, 58, 255};
inline const Color BORDER = {41, 59, 85, 255};
inline const Color CYAN = {56, 189, 248, 255};
inline const Color INDIGO = {129, 140, 248, 255};
inline const Color TEXT = {241, 245, 249, 255};
inline const Color MUTED = {148, 163, 184, 255};
inline const Color BLUE = {59, 130, 246, 255};
inline const Color AMBER = {251, 191, 36, 255};
inline const Color GREEN = {74, 222, 128, 255};
inline const Color PURPLE = {168, 85, 247, 255};
inline const Color ORANGE = {251, 146, 60, 255};
inline const Color RED = {248, 113, 113, 255};

extern "C" {
void InitWindow(int width, int height, const char* title);
void SetWindowMinSize(int width, int height);
void SetTargetFPS(int fps);
bool WindowShouldClose();
void BeginDrawing();
void EndDrawing();
void CloseWindow();
void ClearBackground(Color color);
int GetScreenWidth();
int GetScreenHeight();
float GetMouseWheelMove();

void DrawRectangle(int posX, int posY, int width, int height, Color color);
void DrawRectangleRec(Rectangle rec, Color color);
void DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color);
void DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color);
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color);
void DrawCircleV(Vector2 center, float radius, Color color);
void DrawCircleLines(int centerX, int centerY, float radius, Color color);
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);
void DrawText(const char* text, int posX, int posY, int fontSize, Color color);
int MeasureText(const char* text, int fontSize);
void BeginScissorMode(int x, int y, int width, int height);
void EndScissorMode();

Vector2 GetMousePosition();
bool CheckCollisionPointRec(Vector2 point, Rectangle rec);
bool IsMouseButtonPressed(int button);
bool IsKeyPressed(int key);
int GetCharPressed();
}

constexpr int KEY_BACKSPACE = 259;
constexpr int KEY_ENTER = 257;
constexpr int KEY_RIGHT = 262;
constexpr int KEY_LEFT = 263;
constexpr int KEY_SPACE = 32;
constexpr int KEY_ESCAPE = 256;

#endif
