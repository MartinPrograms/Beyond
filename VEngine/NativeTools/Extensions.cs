using System.Runtime.InteropServices;

namespace NativeTools;

public static class Extensions
{
    public static IntPtr ToPointer(this string str)
    {
        return Marshal.StringToHGlobalAnsi(str);
    }
    
    public static unsafe char* ToCharPointer(this string str)
    {
        return (char*)str.ToPointer();
    }
}