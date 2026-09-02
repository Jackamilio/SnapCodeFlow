local ffi = require("ffi")

ffi.cdef[[
// Detected unknown types, declared as opaque types :
typedef struct rAudioBuffer rAudioBuffer;
typedef struct rAudioProcessor rAudioProcessor;

typedef struct Vector2 { float x; float y; } Vector2;
typedef struct Vector3 { float x; float y; float z; } Vector3;
typedef struct Vector4 { float x; float y; float z; float w; } Vector4;
typedef Vector4 Quaternion;
typedef struct Matrix { float m0; float m4; float m8; float m12; float m1; float m5; float m9; float m13; float m2; float m6; float m10; float m14; float m3; float m7; float m11; float m15; } Matrix;
typedef struct Color { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } Color;
typedef struct Rectangle { float x; float y; float width; float height; } Rectangle;
typedef struct Image { void * data; int width; int height; int mipmaps; int format; } Image;
typedef struct Texture { unsigned int id; int width; int height; int mipmaps; int format; } Texture;
typedef Texture Texture2D;
typedef Texture TextureCubemap;
typedef struct RenderTexture { unsigned int id; Texture texture; Texture depth; } RenderTexture;
typedef RenderTexture RenderTexture2D;
typedef struct NPatchInfo { Rectangle source; int left; int top; int right; int bottom; int layout; } NPatchInfo;
typedef struct GlyphInfo { int value; int offsetX; int offsetY; int advanceX; Image image; } GlyphInfo;
typedef struct Font { int baseSize; int glyphCount; int glyphPadding; Texture2D texture; Rectangle * recs; GlyphInfo * glyphs; } Font;
typedef struct Camera3D { Vector3 position; Vector3 target; Vector3 up; float fovy; int projection; } Camera3D;
typedef Camera3D Camera;
typedef struct Camera2D { Vector2 offset; Vector2 target; float rotation; float zoom; } Camera2D;
typedef struct Mesh { int vertexCount; int triangleCount; float * vertices; float * texcoords; float * texcoords2; float * normals; float * tangents; unsigned char * colors; unsigned short * indices; int boneCount; unsigned char * boneIndices; float * boneWeights; float * animVertices; float * animNormals; unsigned int vaoId; unsigned int * vboId; } Mesh;
typedef struct Shader { unsigned int id; int * locs; } Shader;
typedef struct MaterialMap { Texture2D texture; Color color; float value; } MaterialMap;
typedef struct Material { Shader shader; MaterialMap * maps; float params[4]; } Material;
typedef struct Transform { Vector3 translation; Quaternion rotation; Vector3 scale; } Transform;
typedef Transform *ModelAnimPose;
typedef struct BoneInfo { char name[32]; int parent; } BoneInfo;
typedef struct ModelSkeleton { unsigned int boneCount; BoneInfo * bones; ModelAnimPose bindPose; } ModelSkeleton;
typedef struct Model { Matrix transform; int meshCount; int materialCount; Mesh * meshes; Material * materials; int * meshMaterial; ModelSkeleton skeleton; ModelAnimPose currentPose; Matrix * boneMatrices; } Model;
typedef struct ModelAnimation { char name[32]; unsigned int boneCount; int keyframeCount; ModelAnimPose * keyframePoses; } ModelAnimation;
typedef struct Ray { Vector3 position; Vector3 direction; } Ray;
typedef struct RayCollision { bool hit; float distance; Vector3 point; Vector3 normal; } RayCollision;
typedef struct BoundingBox { Vector3 min; Vector3 max; } BoundingBox;
typedef struct Wave { unsigned int frameCount; unsigned int sampleRate; unsigned int sampleSize; unsigned int channels; void * data; } Wave;
typedef struct AudioStream { rAudioBuffer * buffer; rAudioProcessor * processor; unsigned int sampleRate; unsigned int sampleSize; unsigned int channels; } AudioStream;
typedef struct Sound { AudioStream stream; unsigned int frameCount; } Sound;
typedef struct Music { AudioStream stream; unsigned int frameCount; bool looping; int ctxType; void * ctxData; } Music;
typedef struct VrDeviceInfo { int hResolution; int vResolution; float hScreenSize; float vScreenSize; float eyeToScreenDistance; float lensSeparationDistance; float interpupillaryDistance; float lensDistortionValues[4]; float chromaAbCorrection[4]; } VrDeviceInfo;
typedef struct VrStereoConfig { Matrix projection[2]; Matrix viewOffset[2]; float leftLensCenter[2]; float rightLensCenter[2]; float leftScreenCenter[2]; float rightScreenCenter[2]; float scale[2]; float scaleIn[2]; } VrStereoConfig;
typedef struct FilePathList { unsigned int count; char ** paths; } FilePathList;
typedef struct AutomationEvent { unsigned int frame; unsigned int type; int params[4]; } AutomationEvent;
typedef struct AutomationEventList { unsigned int capacity; unsigned int count; AutomationEvent * events; } AutomationEventList;

typedef enum { // System/Window config flags
    FLAG_VSYNC_HINT               = 64,
    FLAG_FULLSCREEN_MODE          = 2,
    FLAG_WINDOW_RESIZABLE         = 4,
    FLAG_WINDOW_UNDECORATED       = 8,
    FLAG_WINDOW_HIDDEN            = 128,
    FLAG_WINDOW_MINIMIZED         = 512,
    FLAG_WINDOW_MAXIMIZED         = 1024,
    FLAG_WINDOW_UNFOCUSED         = 2048,
    FLAG_WINDOW_TOPMOST           = 4096,
    FLAG_WINDOW_ALWAYS_RUN        = 256,
    FLAG_WINDOW_TRANSPARENT       = 16,
    FLAG_WINDOW_HIGHDPI           = 8192,
    FLAG_WINDOW_MOUSE_PASSTHROUGH = 16384,
    FLAG_BORDERLESS_WINDOWED_MODE = 32768,
    FLAG_MSAA_4X_HINT             = 32,
    FLAG_INTERLACED_HINT          = 65536
} ConfigFlags;

typedef enum { // Trace log level
    LOG_ALL     = 0,
    LOG_TRACE   = 1,
    LOG_DEBUG   = 2,
    LOG_INFO    = 3,
    LOG_WARNING = 4,
    LOG_ERROR   = 5,
    LOG_FATAL   = 6,
    LOG_NONE    = 7
} TraceLogLevel;

typedef enum { // Keyboard keys (US keyboard layout)
    KEY_NULL          = 0,
    KEY_APOSTROPHE    = 39,
    KEY_COMMA         = 44,
    KEY_MINUS         = 45,
    KEY_PERIOD        = 46,
    KEY_SLASH         = 47,
    KEY_ZERO          = 48,
    KEY_ONE           = 49,
    KEY_TWO           = 50,
    KEY_THREE         = 51,
    KEY_FOUR          = 52,
    KEY_FIVE          = 53,
    KEY_SIX           = 54,
    KEY_SEVEN         = 55,
    KEY_EIGHT         = 56,
    KEY_NINE          = 57,
    KEY_SEMICOLON     = 59,
    KEY_EQUAL         = 61,
    KEY_A             = 65,
    KEY_B             = 66,
    KEY_C             = 67,
    KEY_D             = 68,
    KEY_E             = 69,
    KEY_F             = 70,
    KEY_G             = 71,
    KEY_H             = 72,
    KEY_I             = 73,
    KEY_J             = 74,
    KEY_K             = 75,
    KEY_L             = 76,
    KEY_M             = 77,
    KEY_N             = 78,
    KEY_O             = 79,
    KEY_P             = 80,
    KEY_Q             = 81,
    KEY_R             = 82,
    KEY_S             = 83,
    KEY_T             = 84,
    KEY_U             = 85,
    KEY_V             = 86,
    KEY_W             = 87,
    KEY_X             = 88,
    KEY_Y             = 89,
    KEY_Z             = 90,
    KEY_LEFT_BRACKET  = 91,
    KEY_BACKSLASH     = 92,
    KEY_RIGHT_BRACKET = 93,
    KEY_GRAVE         = 96,
    KEY_SPACE         = 32,
    KEY_ESCAPE        = 256,
    KEY_ENTER         = 257,
    KEY_TAB           = 258,
    KEY_BACKSPACE     = 259,
    KEY_INSERT        = 260,
    KEY_DELETE        = 261,
    KEY_RIGHT         = 262,
    KEY_LEFT          = 263,
    KEY_DOWN          = 264,
    KEY_UP            = 265,
    KEY_PAGE_UP       = 266,
    KEY_PAGE_DOWN     = 267,
    KEY_HOME          = 268,
    KEY_END           = 269,
    KEY_CAPS_LOCK     = 280,
    KEY_SCROLL_LOCK   = 281,
    KEY_NUM_LOCK      = 282,
    KEY_PRINT_SCREEN  = 283,
    KEY_PAUSE         = 284,
    KEY_F1            = 290,
    KEY_F2            = 291,
    KEY_F3            = 292,
    KEY_F4            = 293,
    KEY_F5            = 294,
    KEY_F6            = 295,
    KEY_F7            = 296,
    KEY_F8            = 297,
    KEY_F9            = 298,
    KEY_F10           = 299,
    KEY_F11           = 300,
    KEY_F12           = 301,
    KEY_LEFT_SHIFT    = 340,
    KEY_LEFT_CONTROL  = 341,
    KEY_LEFT_ALT      = 342,
    KEY_LEFT_SUPER    = 343,
    KEY_RIGHT_SHIFT   = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT     = 346,
    KEY_RIGHT_SUPER   = 347,
    KEY_KB_MENU       = 348,
    KEY_KP_0          = 320,
    KEY_KP_1          = 321,
    KEY_KP_2          = 322,
    KEY_KP_3          = 323,
    KEY_KP_4          = 324,
    KEY_KP_5          = 325,
    KEY_KP_6          = 326,
    KEY_KP_7          = 327,
    KEY_KP_8          = 328,
    KEY_KP_9          = 329,
    KEY_KP_DECIMAL    = 330,
    KEY_KP_DIVIDE     = 331,
    KEY_KP_MULTIPLY   = 332,
    KEY_KP_SUBTRACT   = 333,
    KEY_KP_ADD        = 334,
    KEY_KP_ENTER      = 335,
    KEY_KP_EQUAL      = 336,
    KEY_BACK          = 4,
    KEY_MENU          = 5,
    KEY_VOLUME_UP     = 24,
    KEY_VOLUME_DOWN   = 25
} KeyboardKey;

typedef enum { // Mouse buttons
    MOUSE_BUTTON_LEFT    = 0,
    MOUSE_BUTTON_RIGHT   = 1,
    MOUSE_BUTTON_MIDDLE  = 2,
    MOUSE_BUTTON_SIDE    = 3,
    MOUSE_BUTTON_EXTRA   = 4,
    MOUSE_BUTTON_FORWARD = 5,
    MOUSE_BUTTON_BACK    = 6
} MouseButton;

typedef enum { // Mouse cursor
    MOUSE_CURSOR_DEFAULT       = 0,
    MOUSE_CURSOR_ARROW         = 1,
    MOUSE_CURSOR_IBEAM         = 2,
    MOUSE_CURSOR_CROSSHAIR     = 3,
    MOUSE_CURSOR_POINTING_HAND = 4,
    MOUSE_CURSOR_RESIZE_EW     = 5,
    MOUSE_CURSOR_RESIZE_NS     = 6,
    MOUSE_CURSOR_RESIZE_NWSE   = 7,
    MOUSE_CURSOR_RESIZE_NESW   = 8,
    MOUSE_CURSOR_RESIZE_ALL    = 9,
    MOUSE_CURSOR_NOT_ALLOWED   = 10
} MouseCursor;

typedef enum { // Gamepad buttons
    GAMEPAD_BUTTON_UNKNOWN          = 0,
    GAMEPAD_BUTTON_LEFT_FACE_UP     = 1,
    GAMEPAD_BUTTON_LEFT_FACE_RIGHT  = 2,
    GAMEPAD_BUTTON_LEFT_FACE_DOWN   = 3,
    GAMEPAD_BUTTON_LEFT_FACE_LEFT   = 4,
    GAMEPAD_BUTTON_RIGHT_FACE_UP    = 5,
    GAMEPAD_BUTTON_RIGHT_FACE_RIGHT = 6,
    GAMEPAD_BUTTON_RIGHT_FACE_DOWN  = 7,
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT  = 8,
    GAMEPAD_BUTTON_LEFT_TRIGGER_1   = 9,
    GAMEPAD_BUTTON_LEFT_TRIGGER_2   = 10,
    GAMEPAD_BUTTON_RIGHT_TRIGGER_1  = 11,
    GAMEPAD_BUTTON_RIGHT_TRIGGER_2  = 12,
    GAMEPAD_BUTTON_MIDDLE_LEFT      = 13,
    GAMEPAD_BUTTON_MIDDLE           = 14,
    GAMEPAD_BUTTON_MIDDLE_RIGHT     = 15,
    GAMEPAD_BUTTON_LEFT_THUMB       = 16,
    GAMEPAD_BUTTON_RIGHT_THUMB      = 17
} GamepadButton;

typedef enum { // Gamepad axes
    GAMEPAD_AXIS_LEFT_X        = 0,
    GAMEPAD_AXIS_LEFT_Y        = 1,
    GAMEPAD_AXIS_RIGHT_X       = 2,
    GAMEPAD_AXIS_RIGHT_Y       = 3,
    GAMEPAD_AXIS_LEFT_TRIGGER  = 4,
    GAMEPAD_AXIS_RIGHT_TRIGGER = 5
} GamepadAxis;

typedef enum { // Material map index
    MATERIAL_MAP_ALBEDO     = 0,
    MATERIAL_MAP_METALNESS  = 1,
    MATERIAL_MAP_NORMAL     = 2,
    MATERIAL_MAP_ROUGHNESS  = 3,
    MATERIAL_MAP_OCCLUSION  = 4,
    MATERIAL_MAP_EMISSION   = 5,
    MATERIAL_MAP_HEIGHT     = 6,
    MATERIAL_MAP_CUBEMAP    = 7,
    MATERIAL_MAP_IRRADIANCE = 8,
    MATERIAL_MAP_PREFILTER  = 9,
    MATERIAL_MAP_BRDF       = 10
} MaterialMapIndex;

typedef enum { // Shader location index
    SHADER_LOC_VERTEX_POSITION          = 0,
    SHADER_LOC_VERTEX_TEXCOORD01        = 1,
    SHADER_LOC_VERTEX_TEXCOORD02        = 2,
    SHADER_LOC_VERTEX_NORMAL            = 3,
    SHADER_LOC_VERTEX_TANGENT           = 4,
    SHADER_LOC_VERTEX_COLOR             = 5,
    SHADER_LOC_MATRIX_MVP               = 6,
    SHADER_LOC_MATRIX_VIEW              = 7,
    SHADER_LOC_MATRIX_PROJECTION        = 8,
    SHADER_LOC_MATRIX_MODEL             = 9,
    SHADER_LOC_MATRIX_NORMAL            = 10,
    SHADER_LOC_VECTOR_VIEW              = 11,
    SHADER_LOC_COLOR_DIFFUSE            = 12,
    SHADER_LOC_COLOR_SPECULAR           = 13,
    SHADER_LOC_COLOR_AMBIENT            = 14,
    SHADER_LOC_MAP_ALBEDO               = 15,
    SHADER_LOC_MAP_METALNESS            = 16,
    SHADER_LOC_MAP_NORMAL               = 17,
    SHADER_LOC_MAP_ROUGHNESS            = 18,
    SHADER_LOC_MAP_OCCLUSION            = 19,
    SHADER_LOC_MAP_EMISSION             = 20,
    SHADER_LOC_MAP_HEIGHT               = 21,
    SHADER_LOC_MAP_CUBEMAP              = 22,
    SHADER_LOC_MAP_IRRADIANCE           = 23,
    SHADER_LOC_MAP_PREFILTER            = 24,
    SHADER_LOC_MAP_BRDF                 = 25,
    SHADER_LOC_VERTEX_BONEIDS           = 26,
    SHADER_LOC_VERTEX_BONEWEIGHTS       = 27,
    SHADER_LOC_MATRIX_BONETRANSFORMS    = 28,
    SHADER_LOC_VERTEX_INSTANCETRANSFORM = 29
} ShaderLocationIndex;

typedef enum { // Shader uniform data type
    SHADER_UNIFORM_FLOAT     = 0,
    SHADER_UNIFORM_VEC2      = 1,
    SHADER_UNIFORM_VEC3      = 2,
    SHADER_UNIFORM_VEC4      = 3,
    SHADER_UNIFORM_INT       = 4,
    SHADER_UNIFORM_IVEC2     = 5,
    SHADER_UNIFORM_IVEC3     = 6,
    SHADER_UNIFORM_IVEC4     = 7,
    SHADER_UNIFORM_UINT      = 8,
    SHADER_UNIFORM_UIVEC2    = 9,
    SHADER_UNIFORM_UIVEC3    = 10,
    SHADER_UNIFORM_UIVEC4    = 11,
    SHADER_UNIFORM_SAMPLER2D = 12
} ShaderUniformDataType;

typedef enum { // Shader attribute data types
    SHADER_ATTRIB_FLOAT = 0,
    SHADER_ATTRIB_VEC2  = 1,
    SHADER_ATTRIB_VEC3  = 2,
    SHADER_ATTRIB_VEC4  = 3
} ShaderAttributeDataType;

typedef enum { // Pixel formats
    PIXELFORMAT_UNCOMPRESSED_GRAYSCALE    = 1,
    PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA   = 2,
    PIXELFORMAT_UNCOMPRESSED_R5G6B5       = 3,
    PIXELFORMAT_UNCOMPRESSED_R8G8B8       = 4,
    PIXELFORMAT_UNCOMPRESSED_R5G5B5A1     = 5,
    PIXELFORMAT_UNCOMPRESSED_R4G4B4A4     = 6,
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8     = 7,
    PIXELFORMAT_UNCOMPRESSED_R32          = 8,
    PIXELFORMAT_UNCOMPRESSED_R32G32B32    = 9,
    PIXELFORMAT_UNCOMPRESSED_R32G32B32A32 = 10,
    PIXELFORMAT_UNCOMPRESSED_R16          = 11,
    PIXELFORMAT_UNCOMPRESSED_R16G16B16    = 12,
    PIXELFORMAT_UNCOMPRESSED_R16G16B16A16 = 13,
    PIXELFORMAT_COMPRESSED_DXT1_RGB       = 14,
    PIXELFORMAT_COMPRESSED_DXT1_RGBA      = 15,
    PIXELFORMAT_COMPRESSED_DXT3_RGBA      = 16,
    PIXELFORMAT_COMPRESSED_DXT5_RGBA      = 17,
    PIXELFORMAT_COMPRESSED_ETC1_RGB       = 18,
    PIXELFORMAT_COMPRESSED_ETC2_RGB       = 19,
    PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA  = 20,
    PIXELFORMAT_COMPRESSED_PVRT_RGB       = 21,
    PIXELFORMAT_COMPRESSED_PVRT_RGBA      = 22,
    PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA  = 23,
    PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA  = 24
} PixelFormat;

typedef enum { // Texture parameters: filter mode
    TEXTURE_FILTER_POINT           = 0,
    TEXTURE_FILTER_BILINEAR        = 1,
    TEXTURE_FILTER_TRILINEAR       = 2,
    TEXTURE_FILTER_ANISOTROPIC_4X  = 3,
    TEXTURE_FILTER_ANISOTROPIC_8X  = 4,
    TEXTURE_FILTER_ANISOTROPIC_16X = 5
} TextureFilter;

typedef enum { // Texture parameters: wrap mode
    TEXTURE_WRAP_REPEAT        = 0,
    TEXTURE_WRAP_CLAMP         = 1,
    TEXTURE_WRAP_MIRROR_REPEAT = 2,
    TEXTURE_WRAP_MIRROR_CLAMP  = 3
} TextureWrap;

typedef enum { // Cubemap layouts
    CUBEMAP_LAYOUT_AUTO_DETECT         = 0,
    CUBEMAP_LAYOUT_LINE_VERTICAL       = 1,
    CUBEMAP_LAYOUT_LINE_HORIZONTAL     = 2,
    CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR = 3,
    CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE = 4
} CubemapLayout;

typedef enum { // Font type, defines generation method
    FONT_DEFAULT = 0,
    FONT_BITMAP  = 1,
    FONT_SDF     = 2
} FontType;

typedef enum { // Color blending modes (pre-defined)
    BLEND_ALPHA             = 0,
    BLEND_ADDITIVE          = 1,
    BLEND_MULTIPLIED        = 2,
    BLEND_ADD_COLORS        = 3,
    BLEND_SUBTRACT_COLORS   = 4,
    BLEND_ALPHA_PREMULTIPLY = 5,
    BLEND_CUSTOM            = 6,
    BLEND_CUSTOM_SEPARATE   = 7
} BlendMode;

typedef enum { // Gesture
    GESTURE_NONE        = 0,
    GESTURE_TAP         = 1,
    GESTURE_DOUBLETAP   = 2,
    GESTURE_HOLD        = 4,
    GESTURE_DRAG        = 8,
    GESTURE_SWIPE_RIGHT = 16,
    GESTURE_SWIPE_LEFT  = 32,
    GESTURE_SWIPE_UP    = 64,
    GESTURE_SWIPE_DOWN  = 128,
    GESTURE_PINCH_IN    = 256,
    GESTURE_PINCH_OUT   = 512
} Gesture;

typedef enum { // Camera system modes
    CAMERA_CUSTOM       = 0,
    CAMERA_FREE         = 1,
    CAMERA_ORBITAL      = 2,
    CAMERA_FIRST_PERSON = 3,
    CAMERA_THIRD_PERSON = 4
} CameraMode;

typedef enum { // Camera projection
    CAMERA_PERSPECTIVE  = 0,
    CAMERA_ORTHOGRAPHIC = 1
} CameraProjection;

typedef enum { // N-patch layout
    NPATCH_NINE_PATCH             = 0,
    NPATCH_THREE_PATCH_VERTICAL   = 1,
    NPATCH_THREE_PATCH_HORIZONTAL = 2
} NPatchLayout;

typedef void (*TraceLogCallback)(int logLevel, const char * text, va_list args);
typedef unsigned char * (*LoadFileDataCallback)(const char * fileName, int * dataSize);
typedef bool (*SaveFileDataCallback)(const char * fileName, const void * data, int dataSize);
typedef char * (*LoadFileTextCallback)(const char * fileName);
typedef bool (*SaveFileTextCallback)(const char * fileName, const char * text);
typedef void (*AudioCallback)(void * bufferData, unsigned int frames);

void InitWindow(int width, int height, const char * title);
void CloseWindow();
bool WindowShouldClose();
bool IsWindowReady();
bool IsWindowFullscreen();
bool IsWindowHidden();
bool IsWindowMinimized();
bool IsWindowMaximized();
bool IsWindowFocused();
bool IsWindowResized();
bool IsWindowState(unsigned int flag);
void SetWindowState(unsigned int flags);
void ClearWindowState(unsigned int flags);
void ToggleFullscreen();
void ToggleBorderlessWindowed();
void MaximizeWindow();
void MinimizeWindow();
void RestoreWindow();
void SetWindowIcon(Image image);
void SetWindowIcons(Image * images, int count);
void SetWindowTitle(const char * title);
void SetWindowPosition(int x, int y);
void SetWindowMonitor(int monitor);
void SetWindowMinSize(int width, int height);
void SetWindowMaxSize(int width, int height);
void SetWindowSize(int width, int height);
void SetWindowOpacity(float opacity);
void SetWindowFocused();
void * GetWindowHandle();
int GetScreenWidth();
int GetScreenHeight();
int GetRenderWidth();
int GetRenderHeight();
int GetMonitorCount();
int GetCurrentMonitor();
Vector2 GetMonitorPosition(int monitor);
int GetMonitorWidth(int monitor);
int GetMonitorHeight(int monitor);
int GetMonitorPhysicalWidth(int monitor);
int GetMonitorPhysicalHeight(int monitor);
int GetMonitorRefreshRate(int monitor);
Vector2 GetWindowPosition();
Vector2 GetWindowScaleDPI();
const char * GetMonitorName(int monitor);
void SetClipboardText(const char * text);
const char * GetClipboardText();
Image GetClipboardImage();
void EnableEventWaiting();
void DisableEventWaiting();
void ShowCursor();
void HideCursor();
bool IsCursorHidden();
void EnableCursor();
void DisableCursor();
bool IsCursorOnScreen();
void ClearBackground(Color color);
void BeginDrawing();
void EndDrawing();
void BeginMode2D(Camera2D camera);
void EndMode2D();
void BeginMode3D(Camera3D camera);
void EndMode3D();
void BeginTextureMode(RenderTexture2D target);
void EndTextureMode();
void BeginShaderMode(Shader shader);
void EndShaderMode();
void BeginBlendMode(int mode);
void EndBlendMode();
void BeginScissorMode(int x, int y, int width, int height);
void EndScissorMode();
void BeginVrStereoMode(VrStereoConfig config);
void EndVrStereoMode();
VrStereoConfig LoadVrStereoConfig(VrDeviceInfo device);
void UnloadVrStereoConfig(VrStereoConfig config);
Shader LoadShader(const char * vsFileName, const char * fsFileName);
Shader LoadShaderFromMemory(const char * vsCode, const char * fsCode);
bool IsShaderValid(Shader shader);
int GetShaderLocation(Shader shader, const char * uniformName);
int GetShaderLocationAttrib(Shader shader, const char * attribName);
void SetShaderValue(Shader shader, int locIndex, const void * value, int uniformType);
void SetShaderValueV(Shader shader, int locIndex, const void * value, int uniformType, int count);
void SetShaderValueMatrix(Shader shader, int locIndex, Matrix mat);
void SetShaderValueTexture(Shader shader, int locIndex, Texture2D texture);
void UnloadShader(Shader shader);
Ray GetScreenToWorldRay(Vector2 position, Camera camera);
Ray GetScreenToWorldRayEx(Vector2 position, Camera camera, int width, int height);
Vector2 GetWorldToScreen(Vector3 position, Camera camera);
Vector2 GetWorldToScreenEx(Vector3 position, Camera camera, int width, int height);
Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera);
Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera);
Matrix GetCameraMatrix(Camera camera);
Matrix GetCameraMatrix2D(Camera2D camera);
void SetTargetFPS(int fps);
float GetFrameTime();
double GetTime();
int GetFPS();
void SwapScreenBuffer();
void PollInputEvents();
void WaitTime(double seconds);
void SetRandomSeed(unsigned int seed);
int GetRandomValue(int min, int max);
int * LoadRandomSequence(unsigned int count, int min, int max);
void UnloadRandomSequence(int * sequence);
void TakeScreenshot(const char * fileName);
void SetConfigFlags(unsigned int flags);
void OpenURL(const char * url);
void SetTraceLogLevel(int logLevel);
void TraceLog(int logLevel, const char * text, ...);
void SetTraceLogCallback(TraceLogCallback callback);
void * MemAlloc(unsigned int size);
void * MemRealloc(void * ptr, unsigned int size);
void MemFree(void * ptr);
unsigned char * LoadFileData(const char * fileName, int * dataSize);
void UnloadFileData(unsigned char * data);
bool SaveFileData(const char * fileName, const void * data, int dataSize);
bool ExportDataAsCode(const unsigned char * data, int dataSize, const char * fileName);
char * LoadFileText(const char * fileName);
void UnloadFileText(char * text);
bool SaveFileText(const char * fileName, const char * text);
void SetLoadFileDataCallback(LoadFileDataCallback callback);
void SetSaveFileDataCallback(SaveFileDataCallback callback);
void SetLoadFileTextCallback(LoadFileTextCallback callback);
void SetSaveFileTextCallback(SaveFileTextCallback callback);
int FileRename(const char * fileName, const char * fileRename);
int FileRemove(const char * fileName);
int FileCopy(const char * srcPath, const char * dstPath);
int FileMove(const char * srcPath, const char * dstPath);
int FileTextReplace(const char * fileName, const char * search, const char * replacement);
int FileTextFindIndex(const char * fileName, const char * search);
bool FileExists(const char * fileName);
bool DirectoryExists(const char * dirPath);
bool IsFileExtension(const char * fileName, const char * ext);
int GetFileLength(const char * fileName);
long GetFileModTime(const char * fileName);
const char * GetFileExtension(const char * fileName);
const char * GetFileName(const char * filePath);
const char * GetFileNameWithoutExt(const char * filePath);
const char * GetDirectoryPath(const char * filePath);
const char * GetPrevDirectoryPath(const char * dirPath);
const char * GetWorkingDirectory();
const char * GetApplicationDirectory();
int MakeDirectory(const char * dirPath);
int ChangeDirectory(const char * dirPath);
bool IsPathFile(const char * path);
bool IsPathDirectory(const char * path);
bool IsPathAbsolute(const char * path);
bool IsFileNameValid(const char * fileName);
FilePathList LoadDirectoryFiles(const char * dirPath);
FilePathList LoadDirectoryFilesEx(const char * basePath, const char * filter, bool scanSubdirs);
void UnloadDirectoryFiles(FilePathList files);
bool IsFileDropped();
FilePathList LoadDroppedFiles();
void UnloadDroppedFiles(FilePathList files);
unsigned int GetDirectoryFileCount(const char * dirPath);
unsigned int GetDirectoryFileCountEx(const char * basePath, const char * filter, bool scanSubdirs);
unsigned char * CompressData(const unsigned char * data, int dataSize, int * compDataSize);
unsigned char * DecompressData(const unsigned char * compData, int compDataSize, int * dataSize);
char * EncodeDataBase64(const unsigned char * data, int dataSize, int * outputSize);
unsigned char * DecodeDataBase64(const char * text, int * outputSize);
unsigned int ComputeCRC32(const unsigned char * data, int dataSize);
unsigned int * ComputeMD5(const unsigned char * data, int dataSize);
unsigned int * ComputeSHA1(const unsigned char * data, int dataSize);
unsigned int * ComputeSHA256(const unsigned char * data, int dataSize);
AutomationEventList LoadAutomationEventList(const char * fileName);
void UnloadAutomationEventList(AutomationEventList list);
bool ExportAutomationEventList(AutomationEventList list, const char * fileName);
void SetAutomationEventList(AutomationEventList * list);
void SetAutomationEventBaseFrame(int frame);
void StartAutomationEventRecording();
void StopAutomationEventRecording();
void PlayAutomationEvent(AutomationEvent event);
bool IsKeyPressed(int key);
bool IsKeyPressedRepeat(int key);
bool IsKeyDown(int key);
bool IsKeyReleased(int key);
bool IsKeyUp(int key);
int GetKeyPressed();
int GetCharPressed();
const char * GetKeyName(int key);
void SetExitKey(int key);
bool IsGamepadAvailable(int gamepad);
const char * GetGamepadName(int gamepad);
bool IsGamepadButtonPressed(int gamepad, int button);
bool IsGamepadButtonDown(int gamepad, int button);
bool IsGamepadButtonReleased(int gamepad, int button);
bool IsGamepadButtonUp(int gamepad, int button);
int GetGamepadButtonPressed();
int GetGamepadAxisCount(int gamepad);
float GetGamepadAxisMovement(int gamepad, int axis);
int SetGamepadMappings(const char * mappings);
void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration);
bool IsMouseButtonPressed(int button);
bool IsMouseButtonDown(int button);
bool IsMouseButtonReleased(int button);
bool IsMouseButtonUp(int button);
int GetMouseX();
int GetMouseY();
Vector2 GetMousePosition();
Vector2 GetMouseDelta();
void SetMousePosition(int x, int y);
void SetMouseOffset(int offsetX, int offsetY);
void SetMouseScale(float scaleX, float scaleY);
float GetMouseWheelMove();
Vector2 GetMouseWheelMoveV();
void SetMouseCursor(int cursor);
int GetTouchX();
int GetTouchY();
Vector2 GetTouchPosition(int index);
int GetTouchPointId(int index);
int GetTouchPointCount();
void SetGesturesEnabled(unsigned int flags);
bool IsGestureDetected(unsigned int gesture);
int GetGestureDetected();
float GetGestureHoldDuration();
Vector2 GetGestureDragVector();
float GetGestureDragAngle();
Vector2 GetGesturePinchVector();
float GetGesturePinchAngle();
void UpdateCamera(Camera * camera, int mode);
void UpdateCameraPro(Camera * camera, Vector3 movement, Vector3 rotation, float zoom);
void SetShapesTexture(Texture2D texture, Rectangle rec);
Texture2D GetShapesTexture();
Rectangle GetShapesTextureRectangle();
void DrawPixel(int posX, int posY, Color color);
void DrawPixelV(Vector2 position, Color color);
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);
void DrawLineV(Vector2 startPos, Vector2 endPos, Color color);
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color);
void DrawLineStrip(const Vector2 * points, int pointCount, Color color);
void DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color);
void DrawLineDashed(Vector2 startPos, Vector2 endPos, int dashSize, int spaceSize, Color color);
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);
void DrawTriangleGradient(Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3);
void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color);
void DrawTriangleFan(const Vector2 * points, int pointCount, Color color);
void DrawTriangleStrip(const Vector2 * points, int pointCount, Color color);
void DrawRectangle(int posX, int posY, int width, int height, Color color);
void DrawRectangleV(Vector2 position, Vector2 size, Color color);
void DrawRectangleRec(Rectangle rec, Color color);
void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color);
void DrawRectangleGradientV(int posX, int posY, int width, int height, Color top, Color bottom);
void DrawRectangleGradientH(int posX, int posY, int width, int height, Color left, Color right);
void DrawRectangleGradientEx(Rectangle rec, Color col1, Color col2, Color col3, Color col4);
void DrawRectangleLines(int posX, int posY, int width, int height, Color color);
void DrawRectangleLinesEx(Rectangle rec, float thick, Color color);
void DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color);
void DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color);
void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float thick, Color color);
void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color);
void DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color);
void DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float thick, Color color);
void DrawCircle(int centerX, int centerY, float radius, Color color);
void DrawCircleV(Vector2 center, float radius, Color color);
void DrawCircleGradient(Vector2 center, float radius, Color inner, Color outer);
void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color);
void DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color);
void DrawCircleLines(int centerX, int centerY, float radius, Color color);
void DrawCircleLinesV(Vector2 center, float radius, Color color);
void DrawCircleLinesEx(Vector2 center, float radius, float thick, Color color);
void DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color);
void DrawEllipseV(Vector2 center, float radiusH, float radiusV, Color color);
void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color);
void DrawEllipseLinesV(Vector2 center, float radiusH, float radiusV, Color color);
void DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);
void DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);
void DrawSplineLinear(const Vector2 * points, int pointCount, float thick, Color color);
void DrawSplineBasis(const Vector2 * points, int pointCount, float thick, Color color);
void DrawSplineCatmullRom(const Vector2 * points, int pointCount, float thick, Color color);
void DrawSplineBezierQuadratic(const Vector2 * points, int pointCount, float thick, Color color);
void DrawSplineBezierCubic(const Vector2 * points, int pointCount, float thick, Color color);
void DrawSplineSegmentLinear(Vector2 p1, Vector2 p2, float thick, Color color);
void DrawSplineSegmentBasis(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float thick, Color color);
void DrawSplineSegmentCatmullRom(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float thick, Color color);
void DrawSplineSegmentBezierQuadratic(Vector2 p1, Vector2 c2, Vector2 p3, float thick, Color color);
void DrawSplineSegmentBezierCubic(Vector2 p1, Vector2 c2, Vector2 c3, Vector2 p4, float thick, Color color);
Vector2 GetSplinePointLinear(Vector2 startPos, Vector2 endPos, float t);
Vector2 GetSplinePointBasis(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float t);
Vector2 GetSplinePointCatmullRom(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float t);
Vector2 GetSplinePointBezierQuadratic(Vector2 p1, Vector2 c2, Vector2 p3, float t);
Vector2 GetSplinePointBezierCubic(Vector2 p1, Vector2 c2, Vector2 c3, Vector2 p4, float t);
bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2);
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2);
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec);
bool CheckCollisionCircleLine(Vector2 center, float radius, Vector2 p1, Vector2 p2);
bool CheckCollisionPointRec(Vector2 point, Rectangle rec);
bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius);
bool CheckCollisionPointTriangle(Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3);
bool CheckCollisionPointLine(Vector2 point, Vector2 p1, Vector2 p2, int threshold);
bool CheckCollisionPointPoly(Vector2 point, const Vector2 * points, int pointCount);
bool CheckCollisionLines(Vector2 startPos1, Vector2 endPos1, Vector2 startPos2, Vector2 endPos2, Vector2 * collisionPoint);
Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2);
Image LoadImage(const char * fileName);
Image LoadImageRaw(const char * fileName, int width, int height, int format, int headerSize);
Image LoadImageAnim(const char * fileName, int * frames);
Image LoadImageAnimFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int * frames);
Image LoadImageFromMemory(const char * fileType, const unsigned char * fileData, int dataSize);
Image LoadImageFromTexture(Texture2D texture);
Image LoadImageFromScreen();
bool IsImageValid(Image image);
void UnloadImage(Image image);
bool ExportImage(Image image, const char * fileName);
unsigned char * ExportImageToMemory(Image image, const char * fileType, int * fileSize);
bool ExportImageAsCode(Image image, const char * fileName);
Image GenImageColor(int width, int height, Color color);
Image GenImageGradientLinear(int width, int height, int direction, Color start, Color end);
Image GenImageGradientRadial(int width, int height, float density, Color inner, Color outer);
Image GenImageGradientSquare(int width, int height, float density, Color inner, Color outer);
Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2);
Image GenImageWhiteNoise(int width, int height, float factor);
Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);
Image GenImageCellular(int width, int height, int tileSize);
Image GenImageText(int width, int height, const char * text);
Image ImageCopy(Image image);
Image ImageFromImage(Image image, Rectangle rec);
Image ImageFromChannel(Image image, int selectedChannel);
Image ImageText(const char * text, int fontSize, Color color);
Image ImageTextEx(Font font, const char * text, float fontSize, float spacing, Color tint);
void ImageFormat(Image * image, int newFormat);
void ImageToPOT(Image * image, Color fill);
void ImageCrop(Image * image, Rectangle crop);
void ImageAlphaCrop(Image * image, float threshold);
void ImageAlphaClear(Image * image, Color color, float threshold);
void ImageAlphaMask(Image * image, Image alphaMask);
void ImageAlphaPremultiply(Image * image);
void ImageBlurGaussian(Image * image, int blurSize);
void ImageKernelConvolution(Image * image, const float * kernel, int kernelSize);
void ImageResize(Image * image, int newWidth, int newHeight);
void ImageResizeNN(Image * image, int newWidth, int newHeight);
void ImageResizeCanvas(Image * image, int newWidth, int newHeight, int offsetX, int offsetY, Color fill);
void ImageMipmaps(Image * image);
void ImageDither(Image * image, int rBpp, int gBpp, int bBpp, int aBpp);
void ImageFlipVertical(Image * image);
void ImageFlipHorizontal(Image * image);
void ImageRotate(Image * image, int degrees);
void ImageRotateCW(Image * image);
void ImageRotateCCW(Image * image);
void ImageColorTint(Image * image, Color color);
void ImageColorInvert(Image * image);
void ImageColorGrayscale(Image * image);
void ImageColorContrast(Image * image, int contrast);
void ImageColorBrightness(Image * image, int brightness);
void ImageColorReplace(Image * image, Color color, Color replace);
Color * LoadImageColors(Image image);
Color * LoadImagePalette(Image image, int maxPaletteSize, int * colorCount);
void UnloadImageColors(Color * colors);
void UnloadImagePalette(Color * colors);
Rectangle GetImageAlphaBorder(Image image, float threshold);
Color GetImageColor(Image image, int x, int y);
void ImageClearBackground(Image * dst, Color color);
void ImageDrawPixel(Image * dst, int posX, int posY, Color color);
void ImageDrawPixelV(Image * dst, Vector2 position, Color color);
void ImageDrawLine(Image * dst, int startPosX, int startPosY, int endPosX, int endPosY, Color color);
void ImageDrawLineV(Image * dst, Vector2 start, Vector2 end, Color color);
void ImageDrawLineEx(Image * dst, Vector2 start, Vector2 end, int thick, Color color);
void ImageDrawLineStrip(Image * dst, const Vector2 * points, int pointCount, Color color);
void ImageDrawTriangle(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);
void ImageDrawTriangleGradient(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3);
void ImageDrawTriangleLines(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);
void ImageDrawTriangleFan(Image * dst, const Vector2 * points, int pointCount, Color color);
void ImageDrawTriangleStrip(Image * dst, const Vector2 * points, int pointCount, Color color);
void ImageDrawRectangle(Image * dst, int posX, int posY, int width, int height, Color color);
void ImageDrawRectangleV(Image * dst, Vector2 position, Vector2 size, Color color);
void ImageDrawRectangleRec(Image * dst, Rectangle rec, Color color);
void ImageDrawRectanglePro(Image * dst, Rectangle rec, Vector2 origin, float rotation, Color color);
void ImageDrawRectangleLines(Image * dst, int posX, int posY, int width, int height, Color color);
void ImageDrawRectangleLinesEx(Image * dst, Rectangle rec, int thick, Color color);
void ImageDrawRectangleGradientEx(Image * dst, Rectangle rec, Color col1, Color col2, Color col3, Color col4);
void ImageDrawCircle(Image * dst, int centerX, int centerY, int radius, Color color);
void ImageDrawCircleV(Image * dst, Vector2 center, int radius, Color color);
void ImageDrawCircleLines(Image * dst, int centerX, int centerY, int radius, Color color);
void ImageDrawCircleLinesV(Image * dst, Vector2 center, int radius, Color color);
void ImageDrawCircleGradient(Image * dst, Vector2 center, float radius, Color inner, Color outer);
void ImageDrawImage(Image * dst, Image src, int posX, int posY, Color tint);
void ImageDrawImageEx(Image * dst, Image src, Vector2 position, float rotation, float scale, Color tint);
void ImageDrawImageRec(Image * dst, Image src, Rectangle srcRec, Vector2 position, Color tint);
void ImageDrawImagePro(Image * dst, Image src, Rectangle srcRec, Rectangle dstRec, Vector2 origin, float rotation, Color tint);
void ImageDrawText(Image * dst, const char * text, int posX, int posY, int fontSize, Color color);
void ImageDrawTextEx(Image * dst, Font font, const char * text, Vector2 position, float fontSize, float spacing, Color tint);
void ImageDrawTextPro(Image * dst, Font font, const char * text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint);
Texture2D LoadTexture(const char * fileName);
Texture2D LoadTextureFromImage(Image image);
TextureCubemap LoadTextureCubemap(Image image, int layout);
RenderTexture2D LoadRenderTexture(int width, int height);
bool IsTextureValid(Texture2D texture);
void UnloadTexture(Texture2D texture);
bool IsRenderTextureValid(RenderTexture2D target);
void UnloadRenderTexture(RenderTexture2D target);
void UpdateTexture(Texture2D texture, const void * pixels);
void UpdateTextureRec(Texture2D texture, Rectangle rec, const void * pixels);
void GenTextureMipmaps(Texture2D * texture);
void SetTextureFilter(Texture2D texture, int filter);
void SetTextureWrap(Texture2D texture, int wrap);
void DrawTexture(Texture2D texture, int posX, int posY, Color tint);
void DrawTextureV(Texture2D texture, Vector2 position, Color tint);
void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint);
void DrawTextureRec(Texture2D texture, Rectangle rec, Vector2 position, Color tint);
void DrawTexturePro(Texture2D texture, Rectangle srcrec, Rectangle dstrec, Vector2 origin, float rotation, Color tint);
void DrawTextureNPatch(Texture2D texture, NPatchInfo nPatchInfo, Rectangle dstrec, Vector2 origin, float rotation, Color tint);
bool ColorIsEqual(Color col1, Color col2);
Color Fade(Color color, float alpha);
int ColorToInt(Color color);
Vector4 ColorNormalize(Color color);
Color ColorFromNormalized(Vector4 normalized);
Vector3 ColorToHSV(Color color);
Color ColorFromHSV(float hue, float saturation, float value);
Color ColorTint(Color color, Color tint);
Color ColorBrightness(Color color, float factor);
Color ColorContrast(Color color, float contrast);
Color ColorAlpha(Color color, float alpha);
Color ColorAlphaBlend(Color dst, Color src, Color tint);
Color ColorLerp(Color color1, Color color2, float factor);
Color GetColor(unsigned int hexValue);
Color GetPixelColor(const void * srcPtr, int format);
void SetPixelColor(void * dstPtr, Color color, int format);
int GetPixelDataSize(int width, int height, int format);
Font GetFontDefault();
Font LoadFont(const char * fileName);
Font LoadFontEx(const char * fileName, int fontSize, const int * codepoints, int codepointCount);
Font LoadFontFromImage(Image image, Color key, int firstChar);
Font LoadFontFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount);
bool IsFontValid(Font font);
GlyphInfo * LoadFontData(const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount, int type, int * glyphCount);
Image GenImageFontAtlas(const GlyphInfo * glyphs, Rectangle ** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod);
void UnloadFontData(GlyphInfo * glyphs, int glyphCount);
void UnloadFont(Font font);
bool ExportFontAsCode(Font font, const char * fileName);
void DrawFPS(int posX, int posY);
void DrawText(const char * text, int posX, int posY, int fontSize, Color color);
void DrawTextEx(Font font, const char * text, Vector2 position, float fontSize, float spacing, Color tint);
void DrawTextPro(Font font, const char * text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint);
void DrawTextCodepoint(Font font, int codepoint, Vector2 position, float fontSize, Color tint);
void DrawTextCodepoints(Font font, const int * codepoints, int codepointCount, Vector2 position, float fontSize, float spacing, Color tint);
void SetTextLineSpacing(int spacing);
int MeasureText(const char * text, int fontSize);
Vector2 MeasureTextEx(Font font, const char * text, float fontSize, float spacing);
Vector2 MeasureTextCodepoints(Font font, const int * codepoints, int length, float fontSize, float spacing);
int GetGlyphIndex(Font font, int codepoint);
GlyphInfo GetGlyphInfo(Font font, int codepoint);
Rectangle GetGlyphAtlasRec(Font font, int codepoint);
char * LoadUTF8(const int * codepoints, int length);
void UnloadUTF8(char * text);
int * LoadCodepoints(const char * text, int * count);
void UnloadCodepoints(int * codepoints);
int GetCodepointCount(const char * text);
int GetCodepoint(const char * text, int * codepointSize);
int GetCodepointNext(const char * text, int * codepointSize);
int GetCodepointPrevious(const char * text, int * codepointSize);
const char * CodepointToUTF8(int codepoint, int * utf8Size);
char ** LoadTextLines(const char * text, int * count);
void UnloadTextLines(char ** text, int lineCount);
int TextCopy(char * dst, const char * src);
bool TextIsEqual(const char * text1, const char * text2);
unsigned int TextLength(const char * text);
const char * TextFormat(const char * text, ...);
const char * TextSubtext(const char * text, int position, int length);
const char * TextRemoveSpaces(const char * text);
char * GetTextBetween(const char * text, const char * begin, const char * end);
char * TextReplace(const char * text, const char * search, const char * replacement);
char * TextReplaceAlloc(const char * text, const char * search, const char * replacement);
char * TextReplaceBetween(const char * text, const char * begin, const char * end, const char * replacement);
char * TextReplaceBetweenAlloc(const char * text, const char * begin, const char * end, const char * replacement);
char * TextInsert(const char * text, const char * insert, int position);
char * TextInsertAlloc(const char * text, const char * insert, int position);
char * TextJoin(char ** textList, int count, const char * delimiter);
char ** TextSplit(const char * text, char delimiter, int * count);
void TextAppend(char * text, const char * append, int * position);
int TextFindIndex(const char * text, const char * search);
char * TextToUpper(const char * text);
char * TextToLower(const char * text);
char * TextToPascal(const char * text);
char * TextToSnake(const char * text);
char * TextToCamel(const char * text);
int TextToInteger(const char * text);
float TextToFloat(const char * text);
void DrawLine3D(Vector3 startPos, Vector3 endPos, Color color);
void DrawPoint3D(Vector3 position, Color color);
void DrawCircle3D(Vector3 center, float radius, Vector3 rotationAxis, float rotationAngle, Color color);
void DrawTriangle3D(Vector3 v1, Vector3 v2, Vector3 v3, Color color);
void DrawTriangleStrip3D(const Vector3 * points, int pointCount, Color color);
void DrawCube(Vector3 position, float width, float height, float length, Color color);
void DrawCubeV(Vector3 position, Vector3 size, Color color);
void DrawCubeWires(Vector3 position, float width, float height, float length, Color color);
void DrawCubeWiresV(Vector3 position, Vector3 size, Color color);
void DrawSphere(Vector3 centerPos, float radius, Color color);
void DrawSphereEx(Vector3 centerPos, float radius, int rings, int slices, Color color);
void DrawSphereWires(Vector3 centerPos, float radius, int rings, int slices, Color color);
void DrawCylinder(Vector3 position, float radiusTop, float radiusBottom, float height, int sides, Color color);
void DrawCylinderEx(Vector3 startPos, Vector3 endPos, float startRadius, float endRadius, int sides, Color color);
void DrawCylinderWires(Vector3 position, float radiusTop, float radiusBottom, float height, int sides, Color color);
void DrawCylinderWiresEx(Vector3 startPos, Vector3 endPos, float startRadius, float endRadius, int sides, Color color);
void DrawCapsule(Vector3 startPos, Vector3 endPos, float radius, int rings, int slices, Color color);
void DrawCapsuleWires(Vector3 startPos, Vector3 endPos, float radius, int rings, int slices, Color color);
void DrawPlane(Vector3 centerPos, Vector2 size, Color color);
void DrawRay(Ray ray, Color color);
void DrawGrid(int slices, float spacing);
Model LoadModel(const char * fileName);
Model LoadModelFromMesh(Mesh mesh);
bool IsModelValid(Model model);
void UnloadModel(Model model);
BoundingBox GetModelBoundingBox(Model model);
void DrawModel(Model model, Vector3 position, float scale, Color tint);
void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);
void DrawModelWires(Model model, Vector3 position, float scale, Color tint);
void DrawModelWiresEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);
void DrawBoundingBox(BoundingBox box, Color color);
void DrawBillboard(Camera camera, Texture2D texture, Vector3 position, float scale, Color tint);
void DrawBillboardRec(Camera camera, Texture2D texture, Rectangle rec, Vector3 position, Vector2 size, Color tint);
void DrawBillboardPro(Camera camera, Texture2D texture, Rectangle rec, Vector3 position, Vector3 up, Vector2 size, Vector2 origin, float rotation, Color tint);
void UploadMesh(Mesh * mesh, bool dynamic);
void UpdateMeshBuffer(Mesh mesh, int index, const void * data, int dataSize, int offset);
void UnloadMesh(Mesh mesh);
void DrawMesh(Mesh mesh, Material material, Matrix transform);
void DrawMeshInstanced(Mesh mesh, Material material, const Matrix * transforms, int instances);
BoundingBox GetMeshBoundingBox(Mesh mesh);
void GenMeshTangents(Mesh * mesh);
bool ExportMesh(Mesh mesh, const char * fileName);
bool ExportMeshAsCode(Mesh mesh, const char * fileName);
Mesh GenMeshPoly(int sides, float radius);
Mesh GenMeshPlane(float width, float length, int resX, int resZ);
Mesh GenMeshCube(float width, float height, float length);
Mesh GenMeshSphere(float radius, int rings, int slices);
Mesh GenMeshHemiSphere(float radius, int rings, int slices);
Mesh GenMeshCylinder(float radius, float height, int slices);
Mesh GenMeshCone(float radius, float height, int slices);
Mesh GenMeshTorus(float radius, float size, int radSeg, int sides);
Mesh GenMeshKnot(float radius, float size, int radSeg, int sides);
Mesh GenMeshHeightmap(Image heightmap, Vector3 size);
Mesh GenMeshCubicmap(Image cubicmap, Vector3 cubeSize);
Material * LoadMaterials(const char * fileName, int * materialCount);
Material LoadMaterialDefault();
bool IsMaterialValid(Material material);
void UnloadMaterial(Material material);
void SetMaterialTexture(Material * material, int mapType, Texture2D texture);
void SetModelMeshMaterial(Model * model, int meshId, int materialId);
ModelAnimation * LoadModelAnimations(const char * fileName, int * animCount);
void UpdateModelAnimation(Model model, ModelAnimation anim, float frame);
void UpdateModelAnimationEx(Model model, ModelAnimation animA, float frameA, ModelAnimation animB, float frameB, float blend);
void UnloadModelAnimations(ModelAnimation * animations, int animCount);
bool IsModelAnimationValid(Model model, ModelAnimation anim);
bool CheckCollisionSpheres(Vector3 center1, float radius1, Vector3 center2, float radius2);
bool CheckCollisionBoxes(BoundingBox box1, BoundingBox box2);
bool CheckCollisionBoxSphere(BoundingBox box, Vector3 center, float radius);
RayCollision GetRayCollisionSphere(Ray ray, Vector3 center, float radius);
RayCollision GetRayCollisionBox(Ray ray, BoundingBox box);
RayCollision GetRayCollisionMesh(Ray ray, Mesh mesh, Matrix transform);
RayCollision GetRayCollisionTriangle(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3);
RayCollision GetRayCollisionQuad(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4);
void InitAudioDevice();
void CloseAudioDevice();
bool IsAudioDeviceReady();
void SetMasterVolume(float volume);
float GetMasterVolume();
Wave LoadWave(const char * fileName);
Wave LoadWaveFromMemory(const char * fileType, const unsigned char * fileData, int dataSize);
bool IsWaveValid(Wave wave);
Sound LoadSound(const char * fileName);
Sound LoadSoundFromWave(Wave wave);
Sound LoadSoundAlias(Sound source);
bool IsSoundValid(Sound sound);
void UpdateSound(Sound sound, const void * data, int frameCount);
void UnloadWave(Wave wave);
void UnloadSound(Sound sound);
void UnloadSoundAlias(Sound alias);
bool ExportWave(Wave wave, const char * fileName);
bool ExportWaveAsCode(Wave wave, const char * fileName);
void PlaySound(Sound sound);
void StopSound(Sound sound);
void PauseSound(Sound sound);
void ResumeSound(Sound sound);
bool IsSoundPlaying(Sound sound);
void SetSoundVolume(Sound sound, float volume);
void SetSoundPitch(Sound sound, float pitch);
void SetSoundPan(Sound sound, float pan);
Wave WaveCopy(Wave wave);
void WaveCrop(Wave * wave, int initFrame, int finalFrame);
void WaveFormat(Wave * wave, int sampleRate, int sampleSize, int channels);
float * LoadWaveSamples(Wave wave);
void UnloadWaveSamples(float * samples);
Music LoadMusicStream(const char * fileName);
Music LoadMusicStreamFromMemory(const char * fileType, const unsigned char * data, int dataSize);
bool IsMusicValid(Music music);
void UnloadMusicStream(Music music);
void PlayMusicStream(Music music);
bool IsMusicStreamPlaying(Music music);
void UpdateMusicStream(Music music);
void StopMusicStream(Music music);
void PauseMusicStream(Music music);
void ResumeMusicStream(Music music);
void SeekMusicStream(Music music, float position);
void SetMusicVolume(Music music, float volume);
void SetMusicPitch(Music music, float pitch);
void SetMusicPan(Music music, float pan);
float GetMusicTimeLength(Music music);
float GetMusicTimePlayed(Music music);
AudioStream LoadAudioStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels);
bool IsAudioStreamValid(AudioStream stream);
void UnloadAudioStream(AudioStream stream);
void UpdateAudioStream(AudioStream stream, const void * data, int frameCount);
bool IsAudioStreamProcessed(AudioStream stream);
void PlayAudioStream(AudioStream stream);
void PauseAudioStream(AudioStream stream);
void ResumeAudioStream(AudioStream stream);
bool IsAudioStreamPlaying(AudioStream stream);
void StopAudioStream(AudioStream stream);
void SetAudioStreamVolume(AudioStream stream, float volume);
void SetAudioStreamPitch(AudioStream stream, float pitch);
void SetAudioStreamPan(AudioStream stream, float pan);
void SetAudioStreamBufferSizeDefault(int size);
void SetAudioStreamCallback(AudioStream stream, AudioCallback callback);
void AttachAudioStreamProcessor(AudioStream stream, AudioCallback processor);
void DetachAudioStreamProcessor(AudioStream stream, AudioCallback processor);
void AttachAudioMixedProcessor(AudioCallback processor);
void DetachAudioMixedProcessor(AudioCallback processor);
]]

local rl = ffi.load("libraylib")
local coltype = ffi.typeof("Color")

local PI = 3.141592653589793
local M = setmetatable({
    Vector2 = ffi.typeof("Vector2"),
    Vector3 = ffi.typeof("Vector3"),
    Vector4 = ffi.typeof("Vector4"),
    Quaternion = ffi.typeof("Quaternion"),
    Matrix = ffi.typeof("Matrix"),
    Color = ffi.typeof("Color"),
    Rectangle = ffi.typeof("Rectangle"),
    Image = ffi.typeof("Image"),
    Texture = ffi.typeof("Texture"),
    Texture2D = ffi.typeof("Texture2D"),
    TextureCubemap = ffi.typeof("TextureCubemap"),
    RenderTexture = ffi.typeof("RenderTexture"),
    RenderTexture2D = ffi.typeof("RenderTexture2D"),
    NPatchInfo = ffi.typeof("NPatchInfo"),
    GlyphInfo = ffi.typeof("GlyphInfo"),
    Font = ffi.typeof("Font"),
    Camera3D = ffi.typeof("Camera3D"),
    Camera = ffi.typeof("Camera"),
    Camera2D = ffi.typeof("Camera2D"),
    Mesh = ffi.typeof("Mesh"),
    Shader = ffi.typeof("Shader"),
    MaterialMap = ffi.typeof("MaterialMap"),
    Material = ffi.typeof("Material"),
    Transform = ffi.typeof("Transform"),
    BoneInfo = ffi.typeof("BoneInfo"),
    ModelSkeleton = ffi.typeof("ModelSkeleton"),
    Model = ffi.typeof("Model"),
    ModelAnimation = ffi.typeof("ModelAnimation"),
    Ray = ffi.typeof("Ray"),
    RayCollision = ffi.typeof("RayCollision"),
    BoundingBox = ffi.typeof("BoundingBox"),
    Wave = ffi.typeof("Wave"),
    AudioStream = ffi.typeof("AudioStream"),
    Sound = ffi.typeof("Sound"),
    Music = ffi.typeof("Music"),
    VrDeviceInfo = ffi.typeof("VrDeviceInfo"),
    VrStereoConfig = ffi.typeof("VrStereoConfig"),
    FilePathList = ffi.typeof("FilePathList"),
    AutomationEvent = ffi.typeof("AutomationEvent"),
    AutomationEventList = ffi.typeof("AutomationEventList"),
    RAYLIB_VERSION_MAJOR = 6,
    RAYLIB_VERSION_MINOR = 1,
    RAYLIB_VERSION_PATCH = 0,
    RAYLIB_VERSION = "6.1-dev",
    PI = 3.141592653589793,
    DEG2RAD = (PI/180.0),
    RAD2DEG = (180.0/PI),
    LIGHTGRAY = coltype( 200, 200, 200, 255 ),
    GRAY = coltype( 130, 130, 130, 255 ),
    DARKGRAY = coltype( 80, 80, 80, 255 ),
    YELLOW = coltype( 253, 249, 0, 255 ),
    GOLD = coltype( 255, 203, 0, 255 ),
    ORANGE = coltype( 255, 161, 0, 255 ),
    PINK = coltype( 255, 109, 194, 255 ),
    RED = coltype( 230, 41, 55, 255 ),
    MAROON = coltype( 190, 33, 55, 255 ),
    GREEN = coltype( 0, 228, 48, 255 ),
    LIME = coltype( 0, 158, 47, 255 ),
    DARKGREEN = coltype( 0, 117, 44, 255 ),
    SKYBLUE = coltype( 102, 191, 255, 255 ),
    BLUE = coltype( 0, 121, 241, 255 ),
    DARKBLUE = coltype( 0, 82, 172, 255 ),
    PURPLE = coltype( 200, 122, 255, 255 ),
    VIOLET = coltype( 135, 60, 190, 255 ),
    DARKPURPLE = coltype( 112, 31, 126, 255 ),
    BEIGE = coltype( 211, 176, 131, 255 ),
    BROWN = coltype( 127, 106, 79, 255 ),
    DARKBROWN = coltype( 76, 63, 47, 255 ),
    WHITE = coltype( 255, 255, 255, 255 ),
    BLACK = coltype( 0, 0, 0, 255 ),
    BLANK = coltype( 0, 0, 0, 0 ),
    MAGENTA = coltype( 255, 0, 255, 255 ),
    RAYWHITE = coltype( 245, 245, 245, 255 ),
}, {
    __index = rl
})return M