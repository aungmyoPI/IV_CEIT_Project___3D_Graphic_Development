#version 330 core
out vec4 FragColor;

in vec3 vDirection;

uniform vec3 topColor;
uniform vec3 horizonColor;
uniform vec3 bottomColor;

void main()
{
    vec3 dir = normalize(vDirection);
    float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);

    vec3 skyColor;
    if (t >= 0.5)
        skyColor = mix(horizonColor, topColor, smoothstep(0.5, 1.0, t));
    else
        skyColor = mix(bottomColor, horizonColor, smoothstep(0.0, 0.5, t));

    vec3 sunDir = normalize(vec3(0.25, 0.55, 0.80));
    float sunAmount = pow(max(dot(dir, sunDir), 0.0), 96.0);
    skyColor += vec3(1.0, 0.92, 0.78) * (sunAmount * 0.25);

    FragColor = vec4(skyColor, 1.0);
}
