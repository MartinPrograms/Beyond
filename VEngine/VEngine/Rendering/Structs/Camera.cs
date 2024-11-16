using System.Numerics;
using System.Runtime.InteropServices;
using VEngine.Tools;

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

    public void Update()
    {
        /*
            glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        // Calculate right vector based on yaw
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

        // Calculate up vector based on right and front
        up = glm::normalize(glm::cross(right, front));

        */
        
        Vector3 newFront;
        newFront.X = (float) Math.Cos(MathHelper.ToRadians(Yaw)) * (float) Math.Cos(MathHelper.ToRadians(Pitch));
        newFront.Y = (float) Math.Sin(MathHelper.ToRadians(Pitch));
        newFront.Z = (float) Math.Sin(MathHelper.ToRadians(Yaw)) * (float) Math.Cos(MathHelper.ToRadians(Pitch));
        Front = Vector3.Normalize(newFront);
        
        Right = Vector3.Normalize(Vector3.Cross(Front, new Vector3(0.0f, 1.0f, 0.0f)));
        Up = Vector3.Normalize(Vector3.Cross(Right, Front));
    }
}