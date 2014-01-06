attribute highp vec4 vertex;
uniform highp mat4 viewMatrix;
uniform float waterHeight;
uniform vec3 cameraLocation;
varying vec3 unprojected;

void main(void)
{
    unprojected = vec3(cameraLocation.x + vertex.x * 500.0, 0.0, cameraLocation.z + vertex.z * 500.0);
    gl_Position = viewMatrix * vec4(unprojected, 1.0);
}
