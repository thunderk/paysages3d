attribute highp vec4 vertex;
uniform highp mat4 viewMatrix;
uniform highp vec3 offset;
uniform float size;
uniform int index;
varying vec3 unprojected;
varying vec2 texcoord;
uniform float waterOffset;

void main(void)
{
    vec3 final = offset + size * (vertex.xyz - vec3(0.0, 0.0, 0.5));  // + vec3(0, waterOffset, 0)
    unprojected = final.xyz;
    texcoord = vec2(0.25 * (vertex.z + float(mod(index, 4))), 0.25 * (vertex.y + float(index / 4)));
    gl_Position = viewMatrix * vec4(final.xyz, 1.0);
}
