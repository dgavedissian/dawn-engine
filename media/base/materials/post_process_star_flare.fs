#version 330 core

in vec2 o_texcoord;

layout(location = 0) out vec4 o_colour;

uniform float aspect_ratio;
uniform vec2 star_location;
uniform float star_radius;

uniform sampler2D in_sampler;

// See the original at: https://www.shadertoy.com/view/4sX3Rs

vec3 glow(vec2 uv, vec2 pos) {
    // Calculate normalized light vector and distance to sphere light surface
    float r = star_radius;
    float distance = length(uv - pos);
    float d = max(distance - r, 0.0);

    // Calculate glow.
	float f0 = 1.0 / pow(d / r + 1.0, 1.0 / 3.0);
    return vec3(f0);
}

vec3 lensflare(vec2 uv,vec2 pos) {
	vec2 uvd = uv*(length(uv));

	float f21 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*0.25;
	float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*0.23;
	float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*0.21;

	vec2 uvx = mix(uv,uvd,-0.5);

	float f41 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
	float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
	float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;

	uvx = mix(uv,uvd,-0.4);

	float f51 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
	float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
	float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;

	uvx = mix(uv,uvd,-0.5);

	float f61 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
	float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
	float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;

	vec3 c = vec3(0.0);
	c.r+=f21+f41+f51+f61;
	c.g+=f22+f42+f52+f62;
	c.b+=f23+f43+f53+f63;
	c = c*0.7 - vec3(length(uvd)*0.0125);

	return c;
}

void main()
{
    vec2 uv = o_texcoord - 0.5;
	vec2 pos = star_location - 0.5;

    uv.x *= aspect_ratio; // fix aspect ratio.
	pos.x *= aspect_ratio; // fix aspect ratio.

	vec3 color =  vec3(1.4, 1.2, 1.0) * (lensflare(uv, pos) + glow(uv, pos));
	o_colour = vec4(color,(color.x + color.y + color.z) / 3.0f);
}