using System.Numerics;
using System.Runtime.InteropServices;

namespace VEngine.Rendering.Structs;

[StructLayout(LayoutKind.Sequential)] // To match the C++ struct
public struct Camera
{
    public Vector3 Position;
    public Vector3 Front;
    public Vector3 Up;
    public Vector3 Right;
    
    public float Yaw;
    public float Pitch;
    public float Roll;
    
    public float Fov;
    
    public float Near;
    public float Far;
    
    public float AspectRatio;
}