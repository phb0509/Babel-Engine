#include "Framework.h"

map<wstring, Shader*> Shader::mTotalShader;

VertexShader* Shader::AddVS(wstring file, string entry)
{
    wstring key = file + ToWString(entry);

    if (mTotalShader.count(key) > 0)
        return (VertexShader*)mTotalShader[key];

    mTotalShader[key] = new VertexShader(file, entry);

    return (VertexShader*)mTotalShader[key];
}

PixelShader* Shader::AddPS(wstring file, string entry)
{
    wstring key = file + ToWString(entry);

    if (mTotalShader.count(key) > 0)
        return (PixelShader*)mTotalShader[key];

    mTotalShader[key] = new PixelShader(file, entry);

    return (PixelShader*)mTotalShader[key];
}

ComputeShader* Shader::AddCS(wstring file, string entry)
{
    wstring key = file + ToWString(entry);

    if (mTotalShader.count(key) > 0)
        return (ComputeShader*)mTotalShader[key];

    mTotalShader[key] = new ComputeShader(file, entry);

    return (ComputeShader*)mTotalShader[key];
}

HullShader* Shader::AddHS(wstring file, string entry)
{
    wstring key = file + ToWString(entry);

    if (mTotalShader.count(key) > 0)
        return (HullShader*)mTotalShader[key];

    mTotalShader[key] = new HullShader(file, entry);

    return (HullShader*)mTotalShader[key];
}

DomainShader* Shader::AddDS(wstring file, string entry)
{
    wstring key = file + ToWString(entry);

    if (mTotalShader.count(key) > 0)
        return (DomainShader*)mTotalShader[key];

    mTotalShader[key] = new DomainShader(file, entry);

    return (DomainShader*)mTotalShader[key];
}

GeometryShader* Shader::AddGS(wstring file, string entry)
{
    wstring key = file + ToWString(entry);

    if (mTotalShader.count(key) > 0)
        return (GeometryShader*)mTotalShader[key];

    mTotalShader[key] = new GeometryShader(file, entry);

    return (GeometryShader*)mTotalShader[key];
}

void Shader::Delete()
{
    for (auto shader : mTotalShader)
    {
        GM->SafeDelete(shader.second);
    }
}
