#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform vec3 material_diffuse;
uniform float fogDensity;
uniform vec3 fogColor;

uniform sampler2D texture_sand;
uniform sampler2D texture_grass;
uniform sampler2D texture_ground;
uniform sampler2D texture_snow;

uniform float terrainMinY;
uniform float terrainMaxY;
uniform float terrainTexTiling;
uniform float terrainWorldTexScale;
uniform float sandMax;
uniform float rockStart;
uniform float rockEnd;
uniform float grassStart;
uniform float grassEnd;
uniform float snowStart;
uniform float snowEnd;
uniform float rockSlopeBoost;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec2 tiledUV = TexCoords * terrainTexTiling;
    vec2 worldUV = FragPos.xz * terrainWorldTexScale;
    // Fallback to world-space mapping when UVs are missing/degenerate.
    float uvVariance = abs(dFdx(TexCoords.x)) + abs(dFdy(TexCoords.y));
    vec2 sampleUV = (uvVariance < 0.00001) ? worldUV : tiledUV;
    vec2 macroUV = sampleUV * 0.12;

    vec3 sandColor = texture(texture_sand, sampleUV).rgb;
    vec3 grassColor = texture(texture_grass, sampleUV).rgb;
    vec3 groundColor = texture(texture_ground, sampleUV).rgb;
    vec3 snowColor = texture(texture_snow, sampleUV).rgb;

    // Low-frequency variation to reduce visible tiling.
    vec3 sandMacro = texture(texture_sand, macroUV).rgb;
    vec3 grassMacro = texture(texture_grass, macroUV).rgb;
    vec3 groundMacro = texture(texture_ground, macroUV).rgb;
    vec3 snowMacro = texture(texture_snow, macroUV).rgb;
    sandColor = mix(sandColor, sandMacro, 0.22);
    grassColor = mix(grassColor, grassMacro, 0.22);
    groundColor = mix(groundColor, groundMacro, 0.22);
    snowColor = mix(snowColor, snowMacro, 0.22);

    float heightSpan = max(terrainMaxY - terrainMinY, 0.001);
    float height01 = clamp((FragPos.y - terrainMinY) / heightSpan, 0.0, 1.0);
    float slope = 1.0 - clamp(dot(norm, vec3(0.0, 1.0, 0.0)), 0.0, 1.0);

    // Elevation-driven layers (top -> bottom: snow, grass, rock, sand).
    float wSand = 1.0 - smoothstep(sandMax * 0.4, sandMax, height01);
    float wGround = smoothstep(rockStart - 0.10, rockStart + 0.10, height01) *
                    (1.0 - smoothstep(rockEnd - 0.10, rockEnd + 0.10, height01));
    float wGrass = smoothstep(grassStart - 0.10, grassStart + 0.10, height01) *
                   (1.0 - smoothstep(grassEnd - 0.10, grassEnd + 0.10, height01));
    float wSnow = smoothstep(snowStart, snowEnd, height01);

    // Steep slopes expose more rock, but do not dominate all bands.
    wGround += slope * rockSlopeBoost;
    wGrass *= (1.0 - slope * 0.45);
    wSnow *= (1.0 - slope * 0.25);

    float weightSum = max(wSand + wGrass + wGround + wSnow, 0.0001);
    vec3 objectColor = (
        sandColor * wSand +
        grassColor * wGrass +
        groundColor * wGround +
        snowColor * wSnow) / weightSum;
        objectColor *= material_diffuse;

    float ambientStrength = 0.34;
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 24.0);
    vec3 specular = 0.06 * spec * lightColor;

    vec3 result = (ambient + 0.95 * diffuse + specular) * objectColor;
    // Simple gamma correction for more natural contrast.
    result = pow(max(result, vec3(0.0)), vec3(1.0 / 2.2));

    float fogDistance = length(viewPos - FragPos);
    float fogAmount = 1.0 - exp(-fogDistance * fogDensity);
    result = mix(result, fogColor, clamp(fogAmount, 0.0, 1.0));

    FragColor = vec4(result, 1.0);
}
