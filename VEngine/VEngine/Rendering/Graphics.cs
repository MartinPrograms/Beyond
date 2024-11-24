using System.Numerics;
using VEngine.Rendering.Managed;

namespace VEngine.Rendering;

/// <summary>
/// Contains all general graphics calls, for example "ClearColor"
/// </summary>
public static class Graphics
{
    private static RenderEngine? _engine;
    
    public static void SetEngine(RenderEngine engine)
    {
        _engine = engine;
    }
    
    public static void ClearColor(float r, float g, float b, float a)
    {
        _engine!.SetClearColor(r, g, b, a);
    }
    
    public static void ClearColor(Vector3 v3)
    {
        ClearColor(v3.X, v3.Y, v3.Z, 1.0f);
    }
    
    public static void CreatePipeline(string name, string vertexShaderPath, string fragmentShaderPath)
    {
        _engine!.CreatePipeline(name, vertexShaderPath, fragmentShaderPath);
    }

    public static RenderEngine GetEngine()
    {
        return _engine!;
    }

    public static unsafe void SetCamera(Camera windowCamera)
    {
        _engine!.SetCamera(windowCamera);
    }
}