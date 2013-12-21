attribute highp vec4 vertex;
uniform highp mat4 viewMatrix;
uniform vec3 cameraLocation;
varying vec3 unprojected;

void main(void)
{
    unprojected = cameraLocation + vertex.xyz * 500.0;
    gl_Position = viewMatrix * vec4(unprojected, 1.0);
}
