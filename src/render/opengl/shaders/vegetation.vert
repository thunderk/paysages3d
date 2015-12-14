in highp vec3 vertex;
in highp vec2 uv;
uniform highp mat4 viewMatrix;
uniform highp vec3 offset;
uniform float size;
out vec3 unprojected;
out vec2 texcoord;
uniform float waterOffset;

void main(void)
{
    unprojected = offset + size * vertex;  // + vec3(0, waterOffset, 0)
    texcoord = uv;
    gl_Position = viewMatrix * vec4(unprojected, 1.0);
}
