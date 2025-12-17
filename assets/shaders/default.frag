#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D aTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
  vec4 textureColor = texture(aTexture, TexCoord);
  FragColor = vec4(textureColor.xyz * objectColor * lightColor, textureColor.w);
}
