attribute highp vec3 vertex;
attribute highp vec2 uv;
uniform highp mat4 viewMatrix;
uniform highp vec3 offset;
uniform float size;
uniform int index;
varying vec3 unprojected;
varying vec2 texcoord;
uniform float waterOffset;

void main(void)
{
    unprojected = offset + size * vertex;  // + vec3(0, waterOffset, 0)
    texcoord = vec2(0.25 * (uv.s + float(mod(index, 4))), 0.25 * (uv.t + float(index / 4)));
    gl_Position = viewMatrix * vec4(unprojected, 1.0);
}
