using System.Numerics;
using VEngine.Rendering.Structs;

namespace VEngine.Rendering.Managed;

public class Camera
{
    SafetyWrapper<Structs.VkCamera> _camera;
    
    public Camera(SafetyWrapper<Structs.VkCamera> camera)
    {
        _camera = camera;
    }

    public unsafe void Update()
    {
        _camera.Pointer->Update();
    }
    
    public static implicit operator SafetyWrapper<Structs.VkCamera>(Camera camera)
    {
        return camera._camera;
    }
    
    public static implicit operator Camera(SafetyWrapper<Structs.VkCamera> camera)
    {
        return new Camera(camera);
    }
    
    public unsafe Vector3 Position
    {
        get => _camera.Pointer->Position;
        set => _camera.Pointer->Position = value;
    }
    
    public unsafe Vector3 Front
    {
        get => _camera.Pointer->Front;
        set => _camera.Pointer->Front = value;
    }
    
    public unsafe Vector3 Up
    {
        get => _camera.Pointer->Up;
        set => _camera.Pointer->Up = value;
    }
    
    public unsafe Vector3 Right
    {
        get => _camera.Pointer->Right;
        set => _camera.Pointer->Right = value;
    }
    
    public unsafe float Yaw
    {
        get => _camera.Pointer->Yaw;
        set => _camera.Pointer->Yaw = value;
    }
    
    public unsafe float Pitch
    {
        get => _camera.Pointer->Pitch;
        set => _camera.Pointer->Pitch = value;
    }
    
    public unsafe float Roll
    {
        get => _camera.Pointer->Roll;
        set => _camera.Pointer->Roll = value;
    }
    
    public unsafe float Fov
    {
        get => _camera.Pointer->Fov;
        set => _camera.Pointer->Fov = value;
    }
    
    public unsafe float Near
    {
        get => _camera.Pointer->Near;
        set => _camera.Pointer->Near = value;
    }
    
    public unsafe float Far
    {
        get => _camera.Pointer->Far;
        set => _camera.Pointer->Far = value;
    }
    
    public unsafe float AspectRatio
    {
        get => _camera.Pointer->AspectRatio;
        set => _camera.Pointer->AspectRatio = value;
    }
}