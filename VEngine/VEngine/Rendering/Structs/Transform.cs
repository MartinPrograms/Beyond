using System.Numerics;
using System.Runtime.InteropServices;

namespace VEngine.Rendering.Structs;

[StructLayout(LayoutKind.Sequential)] // To match the C++ struct
public struct Transform
{
    public Vector3 Position;
    public Quaternion Rotation;
    public Vector3 Scale;
}

public static unsafe class TransformExtensions
{
    public static void Rotate(this SafetyWrapper<Transform> transform, Vector3 axis, float angle)
    {
        transform.Pointer->Rotation *= Quaternion.CreateFromAxisAngle(axis, angle);
    }
}