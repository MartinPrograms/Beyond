using System.Numerics;
using NativeTools;
using VEngine;
using VEngine.Rendering;
using VEngine.Rendering.Structs;
using VEngine.Tools;

string root = System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory, "../../../../../"));

ShaderCompiler.Compile($"{root}/Shaders/default.vert", $"./shaders/vert.spv");
ShaderCompiler.Compile($"{root}/Shaders/default.frag", $"./shaders/frag.spv");

int timeout = 1000;
int timer = 0;
float[] times = new float[1000];

unsafe
{
    var engine = new RenderEngine("VEngine", 800, 600, true, false);
    void* mesh = null;
    SafetyWrapper<Transform> transform = new SafetyWrapper<Transform>(null);

    engine.SetLoadCallback(() =>
    {
        engine.CreatePipeline("test", Path.GetFullPath("./shaders/vert.spv"), Path.GetFullPath("./shaders/frag.spv"));
        mesh = engine.LoadMesh(Path.GetFullPath($"{root}/Models/cube.obj"), 0, "test");
        transform = engine.CreateTransform(); // Because transform is a pointer, we can modify it and it will be reflected in the engine
        

    });

    engine.SetUpdateCallback(() =>
    {
        transform.Rotate(new Vector3(0, 1, 0), 1.0f * engine.GetDeltaTime());
        engine.UpdateGraphics(); 
    });

    engine.SetRenderCallback(() =>
    {
        engine.SetClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        
        engine.SetTransform(mesh, transform);
        engine.RenderMesh(mesh);
        
        timer = (timer + 1) % timeout;
        if (timer == 0)
            Console.WriteLine($"FPS: {1.0f / times.Average()}");
        
        times[timer] = engine.GetDeltaTime();
        
        engine.Render(); 
    });

    engine.Run(); // Blocking call

    engine.Destroy();
}