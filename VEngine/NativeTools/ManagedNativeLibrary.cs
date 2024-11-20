#define __APPLE__

using System.Diagnostics;
using System.Linq.Expressions;
using System.Reflection;
using System.Runtime.InteropServices;
using MulticastDelegate = System.MulticastDelegate;

namespace NativeTools;

public class ManagedNativeLibrary
{
    private IntPtr handle;
    private string path;
    private SymReader symReader;

    [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Auto)]
    private static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr hFile, int dwFlags);

    [DllImport("kernel32", SetLastError = true)]
    private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
    
    // Get last error
    [DllImport("kernel32", SetLastError = true)]
    private static extern int GetLastError();

// Use the correct library name for macOS
    #if __APPLE__
    [DllImport("libdl.dylib", SetLastError = true)]
    #else
    [DllImport("libdl.so.2", SetLastError = true)]
    #endif
    private static extern IntPtr dlopen(string path, int flags);

    #if __APPLE__
    [DllImport("libdl.dylib", SetLastError = true)]
    #else
    [DllImport("libdl.so.2", SetLastError = true)]
    #endif
    private static extern IntPtr dlsym(IntPtr handle, string symbol);

    [DllImport("libdl.so.2", SetLastError = true)]
    private static extern int dlclose(IntPtr handle);

    private const int RTLD_NOW = 2;

    public Dictionary<string, IntPtr> Methods { get; private set; } = new();

    public void Load(string path, bool dumpSymbols = true) // if dumpSymbols is false, it expects a file with the same name as the library with the extension .sym
    {
        string absolutePath = Path.GetFullPath(path);
        
        this.path = absolutePath;
        path = absolutePath;
        
        Console.WriteLine($"Loading library: {path}");
        if (!File.Exists(path))
        {
            throw new FileNotFoundException("Library not found.");
        }
        
        Console.WriteLine($"Library found: {path} with size: {new FileInfo(path).Length} bytes.");
        
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            handle = LoadLibraryEx(path, IntPtr.Zero, 0x0008); // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
        }
        else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux) || RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            handle = dlopen(path, RTLD_NOW);
        }

        if (handle == IntPtr.Zero)
        {
            // Get last error
            int error = GetLastError();
            Console.WriteLine($"Error code: {error}");
            throw new Exception("Failed to load library");
        }

        if (dumpSymbols)
            DumpSymbols();

        // Process the library to load function pointers
        Process();
    }

    /// <summary>
    /// This function uses nm or dumpbin to dump the symbols of the library to a file.
    /// It overwrites the file if it already exists.
    /// </summary>
    private void DumpSymbols()
    {
        symReader = new SymReader(path);
        symReader.DumpSymbols();
    }

    private void Process()
    {
        // Assuming you know the names of the methods you want to load, add them to this list
        List<string> methodNames = new List<string>();
        
        ReadMethodNames(out methodNames);

        foreach (string methodName in methodNames)
        {
            IntPtr methodPtr = GetMethodPointer(methodName);
            if (methodPtr != IntPtr.Zero)
            {
                Methods[methodName] = methodPtr;
            }
            else
            {
                Console.WriteLine($"Warning: Method {methodName} not found in the library.");
            }
        }
    }

    private void ReadMethodNames(out List<string> methodNames)
    {
        if (symReader == null)
        {
            symReader = new SymReader(path);
        }
        
        symReader.ReadMethodNames(out methodNames);
    }

    private IntPtr GetMethodPointer(string methodName)
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            return GetProcAddress(handle, methodName);
        }
        else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux) || RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            return dlsym(handle, methodName);
        }

        throw new PlatformNotSupportedException("Unsupported OS platform.");
    }

    public void Unload()
    {
        if (handle != IntPtr.Zero)
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                FreeLibrary(handle);
            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux) || RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                dlclose(handle);
            }
            handle = IntPtr.Zero;
        }
    }

    [DllImport("kernel32", SetLastError = true)]
    private static extern bool FreeLibrary(IntPtr hModule);
    
    private Dictionary<string, Delegate> cachedDelegates = new();

    // Where V is the delegate type
    public T Invoke<T>(string methodName, params object[] args)
    {
        if (cachedDelegates.ContainsKey(methodName))
        {
            return (T)cachedDelegates[methodName].DynamicInvoke(args);
        }
     
        throw new NotSupportedException("Method not found.");
    }

    public void LoadFunction<T>(string add) where T : Delegate
    {
        IntPtr methodPtr = GetMethodPointer(add);
        if (methodPtr != IntPtr.Zero)
        {
            cachedDelegates[add] = Marshal.GetDelegateForFunctionPointer<T>(methodPtr);
            GCHandle.Alloc(cachedDelegates[add]);
        }
        else
        {
            Console.WriteLine($"Warning: Method {add} not found in the library.");
        }
    }

    public T GetFunction<T>(string s) where T : Delegate
    {
        if (cachedDelegates.ContainsKey(s))
        {
            return (T)cachedDelegates[s];
        }

        throw new NotSupportedException("Method not found.");
    }

    public int CrossPlatformGetLastError()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            return GetLastError();
        }
        else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux) || RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            return Marshal.GetLastWin32Error();
        }

        throw new PlatformNotSupportedException("Unsupported OS platform.");
    }

    public void AutoLoadMethods<T>() where T : class
    {
        // Get all delegates in the class
        // Get member that is of type delegate
        var memberInfos = typeof(T).GetMembers().ToList();
        
        List<Type> delegates = new List<Type>();
        foreach (var memberInfo in memberInfos)
        {
            if (memberInfo.MemberType == MemberTypes.NestedType)
            {
                var nestedType = memberInfo as Type;
                if (nestedType.BaseType.BaseType == typeof(Delegate))
                {
                    delegates.Add(nestedType);
                }
            }
        }
        
        Console.WriteLine($"Found {delegates.Count} delegates in {typeof(T).Name}.");
        
        // To load the function, we must call LoadFunction<T> with the name of the function
        var genericMethod = this.GetType().GetMethod("LoadFunction", BindingFlags.Public | BindingFlags.Instance);
        
        foreach (var field in delegates)
        {
            var method = genericMethod.MakeGenericMethod(field);
            method.Invoke(this, new object[] { field.Name });
            Console.WriteLine($"Loaded method: {field.Name}");
        }
        
        AllocateMethods();
        
        Console.WriteLine("All methods loaded.");
    }

    private void AllocateMethods()
    {
        // Uses GC to allocate the methods
        foreach (var method in Methods)
        {
            GC.KeepAlive(method.Value); // Prevents the GC from collecting the method
        }
        
        foreach (var method in cachedDelegates)
        {
            GC.KeepAlive(method.Value); // Prevents the GC from collecting the method
        }
    }

    public void Dispose()
    {
        Unload();
    }
}