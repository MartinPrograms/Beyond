using NativeTools;
using VEngine;
using VEngine.Rendering;
using VEngine.Tools;

string root = System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory, "../../../../../"));

ShaderCompiler.Compile($"{root}/Shaders/default.vert", $"./shaders/vert.spv");
ShaderCompiler.Compile($"{root}/Shaders/default.frag", $"./shaders/frag.spv");

var engine = new RenderEngine("VEngine", 800, 600, true, false);

engine.SetUpdateCallback(() =>
{
    engine.UpdateGraphics();
});

engine.SetRenderCallback(() =>
{
    engine.Render();
});

engine.CreatePipeline("test", Path.GetFullPath("./shaders/vert.spv"), Path.GetFullPath("./shaders/frag.spv"));

engine.Run(); // Blocking call

engine.Destroy();