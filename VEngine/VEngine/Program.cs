using System.Numerics;
using NativeTools;
using VEngine;
using VEngine.Rendering;
using VEngine.Rendering.Managed;
using VEngine.Rendering.Structs;
using VEngine.Tools;
using Camera = VEngine.Rendering.Managed.Camera;

// I chose to write the Vulkan renderer in C++, because Vulkan is horrible to work with in C#. OpenGL is doable in C#, but doesn't have the same capabilities as Vulkan.
// What you see below will also be abstracted away again, so you do not have to use pointers in C#.
// The C++ code is in the CPP-Vulkan folder in the root of the repository.

string root = System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory, "../../../../../"));

ShaderCompiler.Compile($"{root}/Shaders/default.vert", $"./shaders/vert.spv");
ShaderCompiler.Compile($"{root}/Shaders/default.frag", $"./shaders/frag.spv");

VulkanWindow window = new VulkanWindow("VEngine", 1920, 1080, true, false);
Camera camera = null;

window.Update += deltaTime =>
{
    float speed = 2f * deltaTime;
    if (Input.GetKey(Key.W))
        camera.Position += camera.Front * speed;
    if (Input.GetKey(Key.S))
        camera.Position -= camera.Front * speed;
    if (Input.GetKey(Key.A))
        camera.Position += camera.Right * speed;
    if (Input.GetKey(Key.D))
        camera.Position -= camera.Right * speed;
    if (Input.GetKey(Key.Space))
        camera.Position += camera.Up * speed;
    if (Input.GetKey(Key.LeftShift))
        camera.Position -= camera.Up * speed;


    if (Input.GetMouseButton(MouseButton.Right))
    {
        float sensitivity = 0.1f;
        float x = Input.MouseDelta.X;
        float y = Input.MouseDelta.Y;
        camera.Yaw -= x * sensitivity;
        camera.Pitch -= y * sensitivity;

        camera.Update(); // Update the camera's front, right and up vectors

        Input.MouseMode = MouseMode.Disabled;
    }
    else
    {
        Input.MouseMode = MouseMode.Normal;
    }
};


Mesh m = null;
window.Load += () =>
{
    Graphics.CreatePipeline("default", "./shaders/vert.spv", "./shaders/frag.spv");

    camera = window.MakeCamera();
    camera.Position = new Vector3(0, 0, 3);
    camera.Fov = 60;
    camera.AspectRatio = window.Width / (float) window.Height;
    camera.Near = 0.1f;
    camera.Far = 100f;
    camera.Update();
    
    m = new Mesh(Path.GetFullPath(Path.Combine(root, "Models\\monkey.obj")), 0, "default");
};

var hsv = new Hsv(0, 0.7f, 0.8f);

window.Render += deltaTime =>
{
    hsv.AddHue(25 * deltaTime);
    Graphics.ClearColor(hsv.ToRgb());
    
    Graphics.SetCamera(camera);
    
    m!.Render();
    
    Graphics.SSAO();
};

window.Run();