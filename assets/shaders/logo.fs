#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;
uniform float TIME;
uniform float SCROLL;

void main()
{
   vec2 offset = vec2(TIME * -0.05, TIME * -0.05) * SCROLL;
   FragColor = texture(MAIN_TEXTURE, TexCoord + offset) * COLOR;// * vec4(ourColor, 1.0);
}