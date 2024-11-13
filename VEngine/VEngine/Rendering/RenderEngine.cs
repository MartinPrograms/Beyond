using System.Numerics;
using NativeTools;
using VEngine.Rendering.Structs;

namespace VEngine.Rendering;

public unsafe class RenderEngine
{
    private ManagedNativeLibrary library;
    private void* window;
    private void* context; // graphics context
    
    public RenderEngine(string title, int width, int height, bool vsync, bool fullscreen)
    { 
        library = new ManagedNativeLibrary();
        if (OperatingSystem.IsWindows())
            library.Load("../../../../../CPP-Vulkan/build/Beyond.dll"); // TODO: For macos, use .dylib instead of .dll or .so and remove this hardcoded path lol
        if (OperatingSystem.IsLinux())
            library.Load("../../../../../CPP-Vulkan/build/Beyond.so");
        if (OperatingSystem.IsMacOS())
            library.Load("../../../../../CPP-Vulkan/build/libBeyond.dylib");
        
        library.AutoLoadMethods<Delegates>();
        
        
        window = library.GetFunction<Delegates.CreateWindow>("CreateWindow")(title.ToCharPointer(), width, height, vsync, fullscreen);
        context = library.GetFunction<Delegates.CreateGraphics>("CreateGraphics")();
    }
    
    public void Run()
    {
        library.GetFunction<Delegates.Run>("Run")();
    }
    
    public void SetRenderCallback(Delegates.Callback callback)
    {
        library.GetFunction<Delegates.SetRenderCallback>("SetRenderCallback")(callback);
    }
    
    public void SetUpdateCallback(Delegates.Callback callback)
    {
        library.GetFunction<Delegates.SetUpdateCallback>("SetUpdateCallback")(callback);
    }
    
    public float GetDeltaTime()
    {
        return library.GetFunction<Delegates.GetDeltaTime>("GetDeltaTime")();
    }
    
    public void Destroy()
    {
        library.GetFunction<Delegates.DestroyGraphics>("DestroyGraphics")();
        library.GetFunction<Delegates.DestroyWindow>("DestroyWindow")();
    }
    
    public void Dispose()
    {
        library.Dispose();
    }
    
    public void UpdateGraphics()
    {
        library.GetFunction<Delegates.UpdateGraphics>("UpdateGraphics")();
    }

    public void Render()
    {
        library.GetFunction<Delegates.RenderGraphics>("RenderGraphics")();
    }
    
    public void SetClearColor(float r, float g, float b, float a)
    {
        library.GetFunction<Delegates.SetClearColor>("SetClearColor")(r, g, b, a);
    }

    public void SetClearColor(Vector3 v3)
    {
        SetClearColor(v3.X, v3.Y, v3.Z, 1.0f);
    }
    
    public void CreatePipeline(string name, string vertexShaderPath, string fragmentShaderPath)
    {
        library.GetFunction<Delegates.CreatePipeline>("CreatePipeline")(name.ToCharPointer(), vertexShaderPath.ToCharPointer(), fragmentShaderPath.ToCharPointer());
    }
    
    public void DestroyPipeline(string name)
    {
        library.GetFunction<Delegates.DestroyPipeline>("DestroyPipeline")(name.ToCharPointer());
    }
    
    public void LoadMesh(string path, int index, string pipelineName)
    {
        library.GetFunction<Delegates.LoadMesh>("LoadMesh")(path.ToCharPointer(), index, pipelineName.ToCharPointer());
    }
    
    public void DestroyMesh(void* mesh)
    {
        library.GetFunction<Delegates.DestroyMesh>("DestroyMesh")(mesh);
    }
    
    public void SetTransform(void* mesh, Transform* transform)
    {
        library.GetFunction<Delegates.SetTransform>("SetTransform")(mesh, transform);
    }
    
    public Transform* GetTransform(void* mesh)
    {
        return library.GetFunction<Delegates.GetTransform>("GetTransform")(mesh);
    }
}