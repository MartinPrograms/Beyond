namespace VEngine.Tools;

public static class MathHelper
{
    public const float Pi = 3.14159274f;
    public const float PiOver2 = 1.57079637f;
    public const float PiOver4 = 0.7853982f;
    public const float TwoPi = 6.28318548f;

    public static float ToDegrees(float radians)
    {
        return radians * 57.29578f;
    }

    public static float ToRadians(float degrees)
    {
        return degrees * 0.0174532924f;
    }
}