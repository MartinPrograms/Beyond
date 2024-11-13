namespace VEngine.Rendering;

public class Delegates
{
    public unsafe delegate void* CreateWindow(char* name, int width, int height, bool vsync, bool fullscreen);

    public unsafe delegate void DestroyWindow();
    
    public delegate void Callback();
    public unsafe delegate void SetRenderCallback(Callback renderCallback);
    public unsafe delegate void SetUpdateCallback(Callback updateCallback);
    public unsafe delegate void Run();
    public unsafe delegate float GetDeltaTime();

    /// <summary>
    /// Returns context
    /// </summary>
    public unsafe delegate void* CreateGraphics(); // Uses the existing window
    public unsafe delegate void UpdateGraphics();
    public unsafe delegate void RenderGraphics();
    public unsafe delegate void DestroyGraphics();
}