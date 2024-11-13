using System.Diagnostics;
using System.Runtime.InteropServices;

namespace NativeTools;

public class SymReader
{
    private string path;
    private string symPath;
    
    public SymReader(string path)
    {
        this.path = path;
        this.symPath = path + ".sym";
    }

    public void DumpSymbols()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                FileName = "dumpbin",
                Arguments = $"/exports {path}",
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using (Process process = System.Diagnostics.Process.Start(startInfo))
            {
                using (StreamReader reader = process.StandardOutput)
                {
                    string result = reader.ReadToEnd();
                    File.WriteAllText(symPath, result);
                }
            }
        }
        else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux) || RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                FileName = "nm",
                Arguments = $"-D {path}",
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using (Process process = System.Diagnostics.Process.Start(startInfo))
            {
                using (StreamReader reader = process.StandardOutput)
                {
                    string result = reader.ReadToEnd();
                    File.WriteAllText(symPath, result);
                }
            }
        }
    }

    public void ReadMethodNames(out List<string> methodNames)
    {
        /*
            Windows dumpbin output:
            Microsoft (R) COFF/PE Dumper Version 14.40.33811.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file libpinvokeTesting.dll

File Type: DLL

  Section contains the following exports for libpinvokeTesting.dll

    00000000 characteristics
    6730D6E9 time date stamp Sun Nov 10 16:53:13 2024
        0.00 version
           1 ordinal base
           1 number of functions
           1 number of names

    ordinal hint RVA      name

          1    0 000014B0 add // what we want

  Summary
  // useless stuff
        */
        
        methodNames = new List<string>();
        
        if (OperatingSystem.IsWindows())
        {
            // Assume dumpbin output.
            // Discard the first 15 lines.
            string[] lines = File.ReadAllLines(symPath);
            lines = lines.Skip(15).ToArray();
            foreach (string line in lines)
            {
                string[] parts = line.Split(' ');
                parts = parts.Where(x => !string.IsNullOrEmpty(x)).ToArray();
                
                if (parts.Length >= 4)
                {
                    if (!int.TryParse(parts[0], out _))
                        continue;
                    if (!int.TryParse(parts[1], out _))
                        continue;
                    methodNames.Add(parts[3]);
                }
            }
        }
        
        // TODO: Implement for Linux and OSX
    }
}