attribute highp vec4 vertex;
attribute highp vec2 uv;
uniform highp mat4 viewMatrix;
varying vec3 unprojected;
varying vec2 texcoord;

void main(void)
{
    unprojected = vertex.xyz;
    texcoord = uv;
    gl_Position = viewMatrix * vertex;
}
