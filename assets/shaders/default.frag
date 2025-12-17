#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D aTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);  

  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  float ambientStrength = 1.0;
  vec3 ambient = ambientStrength * lightColor;
  vec4 textureColor = texture(aTexture, TexCoord);
  FragColor = vec4(textureColor.xyz * objectColor * (ambient + diffuse), textureColor.w);
}
