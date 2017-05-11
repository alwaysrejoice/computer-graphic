#version 150

in vec3 vPos;
in vec3 vNormal;
out vec3 pos, norm;

uniform mat4 modelview;
uniform mat4 projection;

void main() {
    pos = vec3(modelview * vec4(vPos, 1.0));
    norm = vec3(modelview * vec4(vNormal, 0.0));
    gl_Position = projection * vec4(pos, 1.0);
}
