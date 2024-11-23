using VEngine.Rendering.Structs;

namespace VEngine.Rendering.Managed;

public unsafe class Mesh
{
    private void* _mesh;
    private SafetyWrapper<Transform> _transform;
    
    public Mesh(string meshPath, int index, string pipelineName)
    {
        var engine = Graphics.GetEngine();
        _mesh = engine.LoadMesh(meshPath, index, pipelineName);
        _transform = engine.CreateTransform();
    }

    public void Render()
    {
        Graphics.GetEngine().RenderMesh(_mesh);
    }
}