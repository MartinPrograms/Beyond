namespace VEngine.Rendering.Structs;

public unsafe class SafetyWrapper<T> where T : unmanaged
{
    public T* Pointer { get; private set; }
    
    public SafetyWrapper(T* pointer)
    {
        Pointer = pointer;
    }
    
    public static implicit operator T*(SafetyWrapper<T> wrapper)
    {
        return wrapper.Pointer;
    }
    
    public static implicit operator SafetyWrapper<T>(T* pointer)
    {
        return new SafetyWrapper<T>(pointer);
    }
}