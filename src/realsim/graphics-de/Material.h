#pragma once
#include <filesystem>

#include "PipelineState.h"
#include "RefCntAutoPtr.hpp"

namespace RSim::Graphics
{
	namespace Dl = Diligent;
	namespace fs = std::filesystem;

    enum class MATERIAL_TYPE
    {
        Surface = 0
    };

	struct MaterialDescription
	{
		fs::path VertexShaderPath;
		fs::path PixelShaderPath;
        MATERIAL_TYPE Type;
	};

    class Material
    {
    public:
        explicit Material(MaterialDescription description) : m_MaterialDesc(std::move(description)) {}

    protected:
        MaterialDescription m_MaterialDesc;
    };


    template<typename ConstantAttribs>
	class SurfaceMaterial : public Material
	{
	public:
        explicit SurfaceMaterial(MaterialDescription description) : Material(description) {}

        ConstantAttribs& Attribs() { return m_Attribs; }
    private:
		Dl::RefCntAutoPtr<Dl::IPipelineState> m_PipelineState;
        Dl::RefCntAutoPtr<Dl::IShaderResourceBinding> m_SRB;
        ConstantAttribs m_Attribs;
    };
}