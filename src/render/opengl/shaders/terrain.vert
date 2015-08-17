attribute highp vec4 vertex;
attribute highp vec2 uv;
uniform highp mat4 viewMatrix;
varying vec3 unprojected;
varying vec2 texcoord;
uniform float waterOffset;

void main(void)
{
    vec4 final = vertex + vec4(0, waterOffset, 0, 0);
    unprojected = final.xyz;
    texcoord = uv;
    gl_Position = viewMatrix * final;
}
