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