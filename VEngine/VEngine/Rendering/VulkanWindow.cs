﻿using VEngine.Rendering.Managed;
using VEngine.Rendering.Structs;

namespace VEngine.Rendering;

public unsafe class VulkanWindow
{
    private RenderEngine _engine;
    private string _title;
    private int _width;
    private int _height;
    private bool _vsync;
    private bool _fullscreen;

    private float[] times = new float[1000];
    private int timer = 0;

    private unsafe SafetyWrapper<VkCamera> camera = new SafetyWrapper<VkCamera>(null); // This is a safety wrapper, which is a pointer to a struct in C++, with some extension methods to make it easier to work with in C#.
    
    public VulkanWindow(string title, int width, int height, bool vsync, bool fullscreen)
    {
        _title = title;
        _width = width;
        _height = height;
        _vsync = vsync;
        _fullscreen = fullscreen;
        _engine = new RenderEngine(title, width, height, vsync, fullscreen);
        
        _engine.SetRenderCallback(_render);
        _engine.SetUpdateCallback(_update);
        _engine.SetLoadCallback(_load);
        
        _engine.DefaultInputCallbacks();
        _engine.InitializeInput();
        
        Graphics.SetEngine(_engine);
    }
    
    ~VulkanWindow()
    {
        _engine.Destroy();
        _engine.Dispose();
    }

    public Action Load;
    public Action<float> Render;
    public Action<float> Update;
    
    public void Run()
    {
        _engine.Run();
    }
    
    private float deltaTime => _engine.GetDeltaTime();
    public unsafe Camera Camera {
        get
        {
            return new Camera(_engine.GetCamera());
        }
    }

    public float Width => _width;
    public float Height => _height;

    private void _render()
    {
        _engine.SetCamera(camera);
        
        Render?.Invoke(deltaTime);
        
        _engine.Render();
    }
    
    private void _load()
    {
        camera = _engine.CreateCamera();
        _engine.SetCamera(camera);
        
        Load?.Invoke();
    }
    
    private void _update()
    {
        Update?.Invoke(deltaTime);
        
        _engine.UpdateGraphics();
        
        times[timer] = deltaTime;
        
        timer = (timer + 1) % times.Length;
        if (timer == 0)
            Console.WriteLine($"FPS: {1.0f / times.Average()}");
    }
}