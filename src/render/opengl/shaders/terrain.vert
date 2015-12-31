in vec4 vertex;
in vec2 uv;
uniform mat4 viewMatrix;
out vec3 unprojected;
out vec2 texcoord;
uniform float waterOffset;

void main(void)
{
    vec4 final = vertex + vec4(0, waterOffset, 0, 0);
    unprojected = final.xyz;
    texcoord = uv;
    gl_Position = viewMatrix * final;
}
