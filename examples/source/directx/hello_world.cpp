#include "examples.h"


static constexpr auto SHADER_SOURCE =
R"(
float4 v_shader(float3 position : KL_Position) : SV_Position
{
    return float4(position, 1.0f);
}

float4 p_shader(float4 position : SV_Position) : SV_Target0
{
    return float4(1.0f, 0.75f, 0.35f, 1.0f);
}
)";

int examples::hello_world_main(const int argc, const char** argv)
{
    kl::Window window{ "Hello World! (D3D11)" };
    kl::GPU gpu{ window };

    window.on_resize.emplace_back([&](const kl::Int2 new_size)
    {
        if (new_size.x > 0 && new_size.y > 0) {
            gpu.resize_internal(new_size);
            gpu.set_viewport_size(new_size);
        }
    });

    const kl::dx::Buffer triangle = gpu.create_vertex_buffer({
        { {  0.0f,  0.5f, 0.5f } },
        { {  0.5f, -0.5f, 0.5f } },
        { { -0.5f, -0.5f, 0.5f } },
    });
    
    const kl::RenderShaders shaders = gpu.create_render_shaders(SHADER_SOURCE);
    gpu.bind_render_shaders(shaders);

    while (window.process()) {
        gpu.clear_internal(kl::Color{ 30, 30, 30 });
        gpu.draw(triangle);
        gpu.swap_buffers(true);
    }
    return 0;
}
