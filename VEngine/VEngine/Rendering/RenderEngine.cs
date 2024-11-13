using NativeTools;

namespace VEngine.Rendering;

public unsafe class RenderEngine
{
    private ManagedNativeLibrary library;
    private void* window;
    private void* context; // graphics context
    
    public RenderEngine(string title, int width, int height, bool vsync, bool fullscreen)
    { 
        library = new ManagedNativeLibrary();
        library.Load("../../../../../CPP-Vulkan/build/Beyond.dll"); // TODO: For macos, use .dylib instead of .dll or .so and remove this hardcoded path lol

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
}