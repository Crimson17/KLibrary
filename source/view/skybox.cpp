#include "view/skybox.h"

#include "utility/file.h"
#include "utility/console.h"


// Skybox box vertices
static const std::vector<kl::vertex> boxVertices
{
	kl::vertex(kl::float3(1, -1,  1)), kl::vertex(kl::float3(1, -1, -1)), kl::vertex(kl::float3(1,  1, -1)),
	kl::vertex(kl::float3(1,  1, -1)), kl::vertex(kl::float3(1,  1,  1)), kl::vertex(kl::float3(1, -1,  1)),
	kl::vertex(kl::float3(-1, -1,  1)), kl::vertex(kl::float3(-1,  1,  1)), kl::vertex(kl::float3(-1,  1, -1)),
	kl::vertex(kl::float3(-1,  1, -1)), kl::vertex(kl::float3(-1, -1, -1)), kl::vertex(kl::float3(-1, -1,  1)),
	kl::vertex(kl::float3(-1,  1,  1)), kl::vertex(kl::float3(1,  1,  1)), kl::vertex(kl::float3(1,  1, -1)),
	kl::vertex(kl::float3(1,  1, -1)), kl::vertex(kl::float3(-1,  1, -1)), kl::vertex(kl::float3(-1,  1,  1)),
	kl::vertex(kl::float3(-1, -1,  1)), kl::vertex(kl::float3(-1, -1, -1)), kl::vertex(kl::float3(1, -1, -1)),
	kl::vertex(kl::float3(1, -1, -1)), kl::vertex(kl::float3(1, -1,  1)), kl::vertex(kl::float3(-1, -1,  1)),
	kl::vertex(kl::float3(-1, -1,  1)), kl::vertex(kl::float3(1, -1,  1)), kl::vertex(kl::float3(1,  1,  1)),
	kl::vertex(kl::float3(1,  1,  1)), kl::vertex(kl::float3(-1,  1,  1)), kl::vertex(kl::float3(-1, -1,  1)),
	kl::vertex(kl::float3(-1, -1, -1)), kl::vertex(kl::float3(-1,  1, -1)), kl::vertex(kl::float3(1,  1, -1)),
	kl::vertex(kl::float3(1,  1, -1)), kl::vertex(kl::float3(1, -1, -1)), kl::vertex(kl::float3(-1, -1, -1))
};

// Shaders
static const std::string vertexShader =
R"(
struct vOut {
    float4 world : SV_POSITION;
    float3 textur : TEX;
};

cbuffer SB_CB : register(b0) {
    matrix vp;
}

vOut vShader(float3 pos : POS_IN, float2 tex : TEX_IN, float3 norm : NORM_IN) {
    vOut data;
    data.world = mul(float4(pos, 0.0f), vp).xyww;
    data.textur = pos;
    return data;
}
)";
static const std::string pixelShader =
R"(
SamplerState samp : register(s0);
TextureCube tex0 : register(t0);

float4 pShader(vOut data) : SV_TARGET {
    return tex0.Sample(samp, data.textur);
}
)";

// Constructors/destructor
kl::skybox::skybox(kl::gpu* gpu, const std::string& name, const kl::image& fullbox)
{
	// Checking the aspect ratio
	if (fullbox.width() % 4 == 0 && fullbox.height() % 3 == 0)
	{
		// Getting the part size
		const int partWidth = fullbox.width() / 4;
		const int partHeight = fullbox.height() / 3;

		// Checking the part size
		if (partWidth == partHeight)
		{
			// Extracting the sides
			const kl::int2 partSize(partWidth, partHeight);
			const kl::image front = fullbox.rect(partSize * kl::int2(1, 1), partSize * kl::int2(2, 2));
			const kl::image back = fullbox.rect(partSize * kl::int2(3, 1), partSize * kl::int2(4, 2));
			const kl::image left = fullbox.rect(partSize * kl::int2(0, 1), partSize * kl::int2(1, 2));
			const kl::image right = fullbox.rect(partSize * kl::int2(2, 1), partSize * kl::int2(3, 2));
			const kl::image top = fullbox.rect(partSize * kl::int2(1, 0), partSize * kl::int2(2, 1));
			const kl::image bottom = fullbox.rect(partSize * kl::int2(1, 2), partSize * kl::int2(2, 3));

			// Calling the other constructor
			this->skybox::skybox(gpu, name, front, back, left, right, top, bottom);
		}
		else
		{
			kl::console::show();
			std::cout << "Skybox image width and height do not match!";
			std::cin.get();
			exit(69);
		}
	}
	else
	{
		kl::console::show();
		std::cout << "Skybox image does not have the correct aspect ratio!";
		std::cin.get();
		exit(69);
	}
}
kl::skybox::skybox(kl::gpu* gpu, const std::string& name, const kl::image& front, const kl::image& back, const kl::image& left, const kl::image& right, const kl::image& top, const kl::image& bottom)
	: gpu(gpu), name(name)
{
	// Compiling skybox shaders
	sky_vtx = gpu->newVertexShader(vertexShader);
	sky_pxl = gpu->newPixelShader(pixelShader);
	vtx_cb = gpu->newConstBuffer(sizeof(kl::mat4));

	// Generating the box mesh
	box_mes = gpu->newVertBuffer(boxVertices);

	// Generating the box texture
	ID3D11Texture2D* boxTex = gpu->newTexture(front, back, left, right, top, bottom);
	box_tex = gpu->newShaderView(boxTex);
	gpu->destroy(boxTex);
}
kl::skybox::~skybox()
{
	gpu->destroy(sky_vtx);
	gpu->destroy(sky_pxl);
	gpu->destroy(vtx_cb);
	gpu->destroy(box_mes);
	gpu->destroy(box_tex);
}

// Renders the cubemap
void kl::skybox::render(const kl::mat4& vpMat) const
{
	// Shader bind
	gpu->bind(sky_pxl);
	gpu->bind(sky_vtx);
	gpu->bindVertCBuff(vtx_cb, 0);
	gpu->setBuffData(vtx_cb, (void*)&vpMat);

	// Binding the texture
	gpu->bindPixlTex(box_tex, 0);

	// Drawing the cubemap
	gpu->draw(box_mes);
}
