using System.Numerics;

namespace VEngine.Tools;

public class Hsv
{
    public float H;
    public float S;
    public float V;
    
    public Hsv(float h, float s, float v)
    {
        H = h;
        S = s;
        V = v;
    }
    
    public void AddHue(float h)
    {
        H += h;
        H %= 360;
    }
    
    public void AddSaturation(float s)
    {
        S += s;
        S = Math.Clamp(S, 0, 1);
    }
    
    public void AddValue(float v)
    {
        V += v;
        V = Math.Clamp(V, 0, 1);
    }
    
    public Vector3 ToRgb()
    {
        float c = V * S;
        float x = c * (1 - MathF.Abs(H / 60 % 2 - 1));
        float m = V - c;
        
        float r = 0;
        float g = 0;
        float b = 0;
        
        if (H >= 0 && H < 60)
        {
            r = c;
            g = x;
        }
        else if (H >= 60 && H < 120)
        {
            r = x;
            g = c;
        }
        else if (H >= 120 && H < 180)
        {
            g = c;
            b = x;
        }
        else if (H >= 180 && H < 240)
        {
            g = x;
            b = c;
        }
        else if (H >= 240 && H < 300)
        {
            r = x;
            b = c;
        }
        else if (H >= 300 && H < 360)
        {
            r = c;
            b = x;
        }
        
        return new Vector3(r + m, g + m, b + m);
    }
    
    public static Hsv FromRgb(Vector3 rgb)
    {
        float max = MathF.Max(rgb.X, MathF.Max(rgb.Y, rgb.Z));
        float min = MathF.Min(rgb.X, MathF.Min(rgb.Y, rgb.Z));
        
        float h = 0;
        float s = 0;
        float v = max;
        
        float c = max - min;
        
        if (c != 0)
        {
            if (max == rgb.X)
            {
                h = 60 * ((rgb.Y - rgb.Z) / c % 6);
            }
            else if (max == rgb.Y)
            {
                h = 60 * ((rgb.Z - rgb.X) / c + 2);
            }
            else if (max == rgb.Z)
            {
                h = 60 * ((rgb.X - rgb.Y) / c + 4);
            }
            
            s = c / v;
        }
        
        return new Hsv(h, s, v);
    }
    
    public override string ToString()
    {
        return $"H: {H}, S: {S}, V: {V}";
    }
}