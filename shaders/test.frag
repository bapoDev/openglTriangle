#version 410 core

in vec3 pos;
uniform float time;
uniform int s_width;
uniform int s_height;
out vec4 frag_color;

void main() {
    // float wild = sin( time ) * 0.5 + 0.5;
    // frag_color.rba = vec3(1.0);
    // frag_color.g = wild;

    frag_color.x = gl_FragCoord.x/1920;
    frag_color.y = gl_FragCoord.y/1080;
    frag_color.zw = gl_FragCoord.zw;
}
