in vec4 vertex;
uniform mat4 viewMatrix;
uniform vec3 cameraLocation;
out vec3 unprojected;

void main(void)
{
    unprojected = cameraLocation + vertex.xyz * 500.0;
    gl_Position = viewMatrix * vec4(unprojected, 1.0);
}
