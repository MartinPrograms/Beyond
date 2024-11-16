using VEngine.Rendering.Structs;

namespace VEngine.Rendering;

public class Delegates
{
    public unsafe delegate void* CreateWindow(char* name, int width, int height, bool vsync, bool fullscreen);

    public unsafe delegate void DestroyWindow();
    
    public delegate void Callback();
    public unsafe delegate void SetRenderCallback(Callback renderCallback);
    public unsafe delegate void SetUpdateCallback(Callback updateCallback);
    public unsafe delegate void SetLoadCallback(Callback loadCallback);
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
}