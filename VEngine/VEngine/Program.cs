using NativeTools;
using VEngine;
using VEngine.Rendering;

var engine = new RenderEngine("VEngine", 800, 600, true, false);

engine.SetUpdateCallback(() =>
{
    // Do nothing rn
});

engine.SetRenderCallback(() =>
{
    Console.WriteLine("FPS: " + 1 / engine.GetDeltaTime());
});

engine.Run();