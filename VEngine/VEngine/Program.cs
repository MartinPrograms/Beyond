using NativeTools;
using VEngine;
using VEngine.Rendering;

var engine = new RenderEngine("VEngine", 800, 600, true, false);

engine.SetUpdateCallback(() =>
{
    engine.UpdateGraphics();
});

engine.SetRenderCallback(() =>
{
    engine.Render();
});

engine.Run();

engine.Destroy();