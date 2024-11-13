namespace VEngine.Tools;

public class ShaderCompiler
{
    /// <summary>
    /// Input and output paths are relative to the project root
    /// </summary>
    /// <param name="input"></param>
    /// <param name="output"></param>
    public static void Compile(string input, string output)
    {
        FileTools.CreateDirectory(output);
        
        System.Diagnostics.Process process = new System.Diagnostics.Process();
        process.StartInfo.FileName = "glslangvalidator";
        process.StartInfo.Arguments = $"-V {input} -o {output}";
        process.StartInfo.UseShellExecute = false;
        process.StartInfo.RedirectStandardOutput = true;
        process.StartInfo.RedirectStandardError = true;
        process.Start();
        process.WaitForExit();
        string outputString = process.StandardOutput.ReadToEnd();
        string errorString = process.StandardError.ReadToEnd();
        if (!string.IsNullOrWhiteSpace(outputString))
        {
            Console.WriteLine(outputString);
        }

        if (!string.IsNullOrWhiteSpace(errorString))
        {
            Console.WriteLine(errorString);
        }
    }
}