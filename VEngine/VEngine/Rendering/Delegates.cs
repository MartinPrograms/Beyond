using System.Numerics;
using VEngine.Rendering.Structs;

namespace VEngine.Rendering;

public class Delegates
{
    public unsafe delegate void* CreateWindow(char* name, int width, int height, bool vsync, bool fullscreen);

    public unsafe delegate void DestroyWindow();
    
    public unsafe delegate void SetRenderCallback(Callbacks.Callback renderCallback);
    public unsafe delegate void SetUpdateCallback(Callbacks.Callback updateCallback);
    public unsafe delegate void SetLoadCallback(Callbacks.Callback loadCallback);
    public unsafe delegate void Run();
    public unsafe delegate float GetDeltaTime();

    /// <summary>
    /// Returns context
    /// </summary>
    public unsafe delegate void* CreateGraphics(); // Uses the existing window
    public unsafe delegate void UpdateGraphics();
    public unsafe delegate void RenderGraphics();
    public unsafe delegate void DestroyGraphics();
    
    public unsafe delegate void SetClearColor(float r, float g, float b, float a);
    
    /// <summary>
    /// Creates a pipeline with the given name and shaders
    /// Shaders must be a path to the compiled shader file.
    /// </summary>
    public unsafe delegate void CreatePipeline(char* name, char* vertexShaderPath, char* fragmentShaderPath);
    public unsafe delegate void DestroyPipeline(char* name);
    
    /// <summary>
    /// Loads a mesh from the given path and returns a pointer to the mesh
    /// Index is the index of the mesh in the file
    /// PipelineName is the name of the pipeline to use
    /// </summary>
    public unsafe delegate void* LoadMesh(char* path, int index, char* pipelineName);
    public unsafe delegate void DestroyMesh(void* mesh);
    
    public unsafe delegate Transform* GetTransform(void* mesh);
    public unsafe delegate void SetTransform(void* mesh, Transform* transform);
    
    // Rendering stuff for meshes
    public unsafe delegate void RenderMesh(void* mesh);

    public unsafe delegate Transform* CreateTransform();

    public unsafe delegate VkCamera* CreateCamera();
    public unsafe delegate void SetCamera(VkCamera* camera);
    public unsafe delegate VkCamera* GetCamera();
    
    // Input:

    
    public unsafe delegate void SetKeyCallback(Callbacks.KeyCallback keyCallback);
    public unsafe delegate void SetMouseButtonCallback(Callbacks.MouseButtonCallback mouseButtonCallback);
    public unsafe delegate void SetMouseCallback(Callbacks.MouseCallback mouseCallback);
    public unsafe delegate void InitializeInput(void* window);

    public unsafe delegate void SetMouseMode(int mode);
    public unsafe delegate void SetResizeCallback(Callbacks.ResizeCallback resizeCallback);

    public unsafe delegate Vector2 *GetResolution();

    public unsafe delegate void UseSSAO();
}

public class Callbacks
{
    public delegate void Callback();
    public unsafe delegate void KeyCallback(int key, int scancode, int action, int mods);
    public unsafe delegate void MouseButtonCallback(int button, int action, int mods);
    public unsafe delegate void MouseCallback(double xpos, double ypos);
    public unsafe delegate void ResizeCallback(int width, int height);
}