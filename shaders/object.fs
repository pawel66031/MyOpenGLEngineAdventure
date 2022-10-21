#version 330 core

struct Material {
    // vec3 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform sampler2D diffuse0;
uniform sampler2D specular0;

#define MAX_POINT_LIGHTS 20
struct PointLight {
    vec3 position;

    float k0;
    float k1;
    float k2;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

struct DirectLight {
    vec3 direction;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform DirectLight directLight;

#define MAX_SPOT_LIGHTS 5
struct SpotLight {
    vec3 position;
    vec3 direction;

    float k0;
    float k1;
    float k2;

    float cutOff;
    float outerCutOff;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;
uniform int noTex;

uniform vec3 viewPos;

vec4 calcDirectLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);


void main(){
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 diffMap;
    vec4 specMap;

    if(noTex == 1){
        diffMap = material.diffuse;
        specMap = material.specular;
    } else {
        diffMap = texture(diffuse0, TexCoord);
        specMap = texture(specular0, TexCoord);
    }

    // placeholder
    vec4 result;

    // directional
    result = calcDirectLight(norm, viewDir, diffMap, specMap);

    // point lights
    for(int i = 0; i < noPointLights; ++i){
        result += calcPointLight(i, norm, viewDir, diffMap, specMap);
    }

    // spot lights
    for(int i = 0; i < noSpotLights; ++i){
        result += calcSpotLight(i, norm, viewDir, diffMap, specMap);
    }

    FragColor = result;
    // FragColor = vec4(calcSpotLight(norm, viewDir, diffMap, specMap), 1.0);
    // FragColor = vec4(calcDirectLight(norm, viewDir, diffMap, specMap), 1.0);
}


vec4 calcDirectLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap){
    // ambient
    vec4 ambient = directLight.ambient * diffMap;

    // diffuse
    vec3 lightDir = normalize(-directLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = directLight.diffuse * (diff * diffMap);

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    vec4 specular = directLight.specular * (spec * specMap);

    return vec4(ambient + diffuse + specular);
}

vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap){
    // ambient
    vec4 ambient = pointLights[idx].ambient * diffMap;

    // diffuse
    // vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLights[idx].position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = pointLights[idx].diffuse * (diff * diffMap);

    // specular
    // vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    vec4 specular = pointLights[idx].specular * (spec * specMap);

    float dist = length(pointLights[idx].position - FragPos);
    float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * (dist * dist));

    return vec4(ambient + diffuse + specular);
}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap){
    vec3 lightDir = normalize(spotLights[idx].position - FragPos);
    float theta = dot(lightDir, normalize(-spotLights[idx].direction));

    //ambient
    vec4 ambient = spotLights[idx].ambient * diffMap;

    if (theta > spotLights[idx].outerCutOff){ // because using cosines and not degrees
        // if in cutoff, render light

        // diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec4 diffuse = spotLights[idx].diffuse * (diff * diffMap);

        // specular
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        vec4 specular = spotLights[idx].specular * (spec * specMap);

        float intensity = (theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff);
        intensity = clamp(intensity, 0.0, 1.0);
        diffuse *= intensity;
        specular *= intensity;

        float dist = length(spotLights[idx].position - FragPos);
        float attenuation = 1.0 / (spotLights[idx].k0 + spotLights[idx].k1 * dist + spotLights[idx].k2 * (dist * dist));

        return vec4(ambient + diffuse + specular) * attenuation;
    } else {
        // render just ambient
        return ambient;
    }
}