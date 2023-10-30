#version 330 core

// Data from vertex shader.
// --------------------------------------------------------
// Add your data for interpolation.
// --------------------------------------------------------
in vec3 iPosWorld;
in vec3 iNormalWorld;
in vec2 iTexCoord;
// --------------------------------------------------------
// Add your uniform variables.
// --------------------------------------------------------
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform vec3 cameraPos;
uniform vec3 ambientLight;
// directional Light
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
// point Light
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;
// spot Light
uniform vec3 spotLightPos;
uniform vec3 spotLightIntensity;
uniform vec3 spotLightDir;
uniform float spotLightCutOffStart;
uniform float spotLightTotalWidth;

//texture
uniform sampler2D mapKd;
uniform bool havemapKd;

out vec4 FragColor;

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir) {
    return Kd * I * max(0, dot(N, lightDir));
}
vec3 Specular(vec3 Ks,vec3 I,vec3 N,vec3 vL,vec3 vE,float Ns){
    vec3 vH = normalize(vL + vE);
    return Ks * I * pow( max(0 , dot(vH, N)), Ns);
}
void main()
{
    vec3 fsNormal = normalize(iNormalWorld);
    vec3 Kdd = Kd;
    if(havemapKd){
        Kdd = texture2D(mapKd,iTexCoord).rgb;
    }
    // Ambient Light
    vec3 ambient = Ka * ambientLight;
    vec3 vE = normalize(cameraPos - iPosWorld);
    // 
    // Directional Light
    vec3 dirLightvL = normalize(-dirLightDir); 
    vec3 dirdiffuse = Diffuse(Kdd, dirLightRadiance, fsNormal, dirLightvL);
    vec3 dirspecular = Specular( Ks, dirLightRadiance, fsNormal, dirLightvL, vE, Ns);
    vec3 dirLight =  dirdiffuse + dirspecular; 
    // 
    // Point Light 
    vec3 pointLightvL = normalize(pointLightPos - iPosWorld);
    float disttoPointLight = distance(pointLightPos, iPosWorld);
    float pointattenuation = 1.0f / (disttoPointLight * disttoPointLight);
    vec3 pointRadiance = pointattenuation * pointLightIntensity;
    vec3 pointdiffuse = Diffuse(Kdd, pointRadiance, fsNormal, pointLightvL);
    vec3 pointspecular = Specular( Ks, pointRadiance, fsNormal, pointLightvL, vE, Ns);
    vec3 pointLight = pointdiffuse + pointspecular;
    // 
    // Spot Light
    vec3 spotLightvL = normalize(spotLightPos - iPosWorld);
    float theta = dot(-spotLightvL,normalize(spotLightDir));
    float disttoSpotLight = distance(spotLightPos, iPosWorld);
    float spotattenuation =  1.0f / (disttoSpotLight * disttoSpotLight);
    float attenuation = clamp((theta - spotLightTotalWidth) / (spotLightCutOffStart - spotLightTotalWidth), 0.0, 1.0);
    spotattenuation *= attenuation;
    vec3 spotRadiance = spotLightIntensity * spotattenuation;
    vec3 spotdiffuse = Diffuse(Kdd, spotRadiance, fsNormal, spotLightvL);
    vec3 spotspecular = Specular( Ks, spotRadiance , fsNormal, spotLightvL, vE, Ns);
    vec3 spotLight = spotdiffuse + spotspecular;

    vec3 LightColor = ambient + dirLight + pointLight + spotLight;
    FragColor = vec4(LightColor,1.0);
}