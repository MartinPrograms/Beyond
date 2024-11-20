using System.Numerics;
using System.Runtime.InteropServices;
using VEngine.Tools;

namespace VEngine.Rendering.Structs;

[StructLayout(LayoutKind.Sequential)] // To match the C++ struct
public struct VkCamera
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

    public void Update()
    {
        Vector3 newFront;
        newFront.X = (float) Math.Cos(MathHelper.ToRadians(Yaw)) * (float) Math.Cos(MathHelper.ToRadians(Pitch));
        newFront.Y = (float) Math.Sin(MathHelper.ToRadians(Pitch));
        newFront.Z = (float) Math.Sin(MathHelper.ToRadians(Yaw)) * (float) Math.Cos(MathHelper.ToRadians(Pitch));
        Front = Vector3.Normalize(newFront);
        
        Right = Vector3.Normalize(Vector3.Cross(Front, new Vector3(0.0f, 1.0f, 0.0f)));
        Up = Vector3.Normalize(Vector3.Cross(Right, Front));
    }
}