namespace VEngine.Tools;

public class FileTools
{
    public static void CreateDirectory(string path)
    {
        string directory = System.IO.Path.GetDirectoryName(path);
        if (!System.IO.Directory.Exists(directory))
        {
            System.IO.Directory.CreateDirectory(directory);
        }
    }
}