using System.Numerics;
using System.Runtime.InteropServices;
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
            library.Load(
                "../../../../../CPP-Vulkan/build/Beyond.dll"); // TODO: For macos, use .dylib instead of .dll or .so and remove this hardcoded path lol
        if (OperatingSystem.IsLinux())
            library.Load("../../../../../CPP-Vulkan/build/Beyond.so");
        if (OperatingSystem.IsMacOS())
            library.Load("../../../../../CPP-Vulkan/build/libBeyond.dylib");

        library.AutoLoadMethods<Delegates>();


        window = library.GetFunction<Delegates.CreateWindow>("CreateWindow")(title.ToCharPointer(), width, height,
            vsync, fullscreen);
        context = library.GetFunction<Delegates.CreateGraphics>("CreateGraphics")();
    }

    private Delegates.Run _run;
    public void Run()
    {
        _run = library.GetFunction<Delegates.Run>("Run");
        _run();
    }
    
    public void SetRenderCallback(Callbacks.Callback callback)
    {
        GCHandle.Alloc(callback);
        library.GetFunction<Delegates.SetRenderCallback>("SetRenderCallback")(callback);
    }
    
    public void SetUpdateCallback(Callbacks.Callback callback)
    {
        GCHandle.Alloc(callback);
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
        
        Input.Update();
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
    
    public void* LoadMesh(string path, int index, string pipelineName)
    {
        return library.GetFunction<Delegates.LoadMesh>("LoadMesh")(path.ToCharPointer(), index, pipelineName.ToCharPointer());
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

    public void SetLoadCallback(Callbacks.Callback action)
    {
        GCHandle.Alloc(action); // Prevent garbage collection
        library.GetFunction<Delegates.SetLoadCallback>("SetLoadCallback")(action);
    }

    public void RenderMesh(void* mesh)
    {
        library.GetFunction<Delegates.RenderMesh>("RenderMesh")(mesh);
    }

    public SafetyWrapper<Transform> CreateTransform()
    {
        return new SafetyWrapper<Transform>(library.GetFunction<Delegates.CreateTransform>("CreateTransform")());
    }

    public SafetyWrapper<VkCamera> CreateCamera()
    {
        return new SafetyWrapper<VkCamera>(library.GetFunction<Delegates.CreateCamera>("CreateCamera")());
    }
    public void SetCamera(VkCamera* camera)
    {
        library.GetFunction<Delegates.SetCamera>("SetCamera")(camera);
    }
    
    public VkCamera* GetCamera()
    {
        return library.GetFunction<Delegates.GetCamera>("GetCamera")();
    }

    public void SetKeyCallback(Callbacks.KeyCallback action)
    {
        library.GetFunction<Delegates.SetKeyCallback>("SetKeyCallback")(action);
    }
    
    public void SetMouseButtonCallback(Callbacks.MouseButtonCallback action)
    {
        library.GetFunction<Delegates.SetMouseButtonCallback>("SetMouseButtonCallback")(action);
    }
    
    public void SetMouseCallback(Callbacks.MouseCallback action)
    {
        library.GetFunction<Delegates.SetMouseCallback>("SetMouseCallback")(action);
    }

    public void InitializeInput()
    {
        library.GetFunction<Delegates.InitializeInput>("InitializeInput")(window);
    }

    public void DefaultInputCallbacks()
    {
        keyCallback = KeyCallback;
        mouseButtonCallback = MouseButtonCallback;
        mouseCallback = MouseCallback;
        
        SetKeyCallback(keyCallback);
        SetMouseButtonCallback(mouseButtonCallback);
        SetMouseCallback(mouseCallback);
        
        Input.OnMouseModeChanged += (sender) =>
        {
            library.GetFunction<Delegates.SetMouseMode>("SetMouseMode")((int)Input.MouseMode);
        };
    }
    
    private void MouseCallback(double xpos, double ypos)
    {
        Input.SetMousePosition(new Vector2((float)xpos, (float)ypos));
    }

    private void MouseButtonCallback(int button, int action, int mods)
    {
        if (button > 4) return;
        
        if (action == 1)
            Input.SetMouseButtonDown((MouseButton)button);
        else
            Input.SetMouseButtonUp((MouseButton)button);
    }

    private void KeyCallback(int key, int scancode, int action, int mods)
    {
        try
        {
            Key k = (Key)key;
            if (action == 1 || action == 2)
                Input.SetKeyDown(k);
            else
                Input.SetKeyUp(k);
        }
        catch
        {
            // ignored
        }
    }

    private Callbacks.KeyCallback keyCallback;
    private Callbacks.MouseButtonCallback mouseButtonCallback;
    private Callbacks.MouseCallback mouseCallback;

    public void SetResizeCallback(Callbacks.ResizeCallback action)
    {
        library.GetFunction<Delegates.SetResizeCallback>("SetResizeCallback")(action);
    }

    public Vector2 *GetResolution()
    {
        return (library.GetFunction<Delegates.GetResolution>("GetResolution")());
    }
    
    public void* CreatePostProcessingEffect(PostProcessingType type, string vertexShaderPath, string fragmentShaderPath)
    {
        return library.GetFunction<Delegates.CreatePostProcessingEffect>("CreatePostProcessingEffect")((int)type, vertexShaderPath.ToCharPointer(), fragmentShaderPath.ToCharPointer());
    }
    
    public void UsePostProcessingEffect(void* effect)
    {
        library.GetFunction<Delegates.UsePostProcessingEffect>("UsePostProcessingEffect")(effect);
    }

    public enum PostProcessingType
    {
        Blur,
        Ssao,
    }
}