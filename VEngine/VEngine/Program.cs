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
    SafetyWrapper<Camera> camera = new SafetyWrapper<Camera>(null);
    
    engine.DefaultInputCallbacks();

    engine.SetResizeCallback((int x, int y) =>
    {
        camera.Pointer->AspectRatio = (float)x / y;
    });

    engine.SetLoadCallback(() =>    
    {
        engine.InitializeInput();
        
        engine.CreatePipeline("test", Path.GetFullPath("./shaders/vert.spv"), Path.GetFullPath("./shaders/frag.spv"));
        mesh = engine.LoadMesh(Path.GetFullPath($"{root}/Models/monkey.obj"), 0, "test");
        transform = engine.CreateTransform();
        camera = engine.CreateCamera();
        
        Vector2* res = engine.GetResolution();
        Console.WriteLine($"Resolution: {res->X}x{res->Y}");
        camera.Pointer->AspectRatio = (float)res->X / res->Y;
    });

    engine.SetUpdateCallback(() =>
    {
        float speed = 2f * engine.GetDeltaTime();
        if (Input.GetKey(Key.W))
            camera.Pointer->Position += camera.Pointer->Front * speed;
        if (Input.GetKey(Key.S))
            camera.Pointer->Position -= camera.Pointer->Front * speed;
        if (Input.GetKey(Key.A))
            camera.Pointer->Position += camera.Pointer->Right * speed;
        if (Input.GetKey(Key.D))
            camera.Pointer->Position -= camera.Pointer->Right * speed;
        if (Input.GetKey(Key.Space))
            camera.Pointer->Position += camera.Pointer->Up * speed;
        if (Input.GetKey(Key.LeftShift))
            camera.Pointer->Position -= camera.Pointer->Up * speed;
        

        if (Input.GetMouseButton(MouseButton.Right))
        {
            float sensitivity = 0.1f;
            float x = Input.MouseDelta.X;
            float y = Input.MouseDelta.Y;
            camera.Pointer->Yaw -= x * sensitivity;
            camera.Pointer->Pitch -= y * sensitivity;

            camera.Pointer->Update();
            
            Input.MouseMode = MouseMode.Disabled;
        }
        else
        {
            Input.MouseMode = MouseMode.Normal;
        }
        
        engine.UpdateGraphics(); 
    });

    engine.SetRenderCallback(() =>
    {
        engine.SetClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        
        engine.SetCamera(camera);
        
        engine.SetTransform(mesh, transform);
        engine.RenderMesh(mesh);
        
        timer = (timer + 1) % timeout;
        if (timer == 0)
            Console.WriteLine($"FPS: {1.0f / times.Average()}");
        
        times[timer] = engine.GetDeltaTime();
        
        engine.Render(); // All the functions above (apart from deltaTimes, and setTransforms) are adding to a queue, this function processes the queue
    });

    engine.Run(); // Blocking call

    engine.Destroy();
}