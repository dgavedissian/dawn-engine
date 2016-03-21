#version 330 core

uniform sampler2D texture;
uniform float radius;
uniform vec2 screenSize;
uniform vec2 dir;

in vec2 vTexCoord;

out vec4 colour;

void main()
{
    vec2 tc = vTexCoord;
    float blur = radius / screenSize.x;

    float hstep = dir.x;
    float vstep = dir.y;

    colour += texture(texture, vec2(tc.x - 4.0 * blur * hstep, tc.y - 4.0 * blur * vstep)) * 0.0162162162;
    colour += texture(texture, vec2(tc.x - 3.0 * blur * hstep, tc.y - 3.0 * blur * vstep)) * 0.0540540541;
    colour += texture(texture, vec2(tc.x - 2.0 * blur * hstep, tc.y - 2.0 * blur * vstep)) * 0.1216216216;
    colour += texture(texture, vec2(tc.x - 1.0 * blur * hstep, tc.y - 1.0 * blur * vstep)) * 0.1945945946;

    colour += texture(texture, vec2(tc.x, tc.y)) * 0.2270270270;

    colour += texture(texture, vec2(tc.x + 1.0 * blur * hstep, tc.y + 1.0 * blur * vstep)) * 0.1945945946;
    colour += texture(texture, vec2(tc.x + 2.0 * blur * hstep, tc.y + 2.0 * blur * vstep)) * 0.1216216216;
    colour += texture(texture, vec2(tc.x + 3.0 * blur * hstep, tc.y + 3.0 * blur * vstep)) * 0.0540540541;
    colour += texture(texture, vec2(tc.x + 4.0 * blur * hstep, tc.y + 4.0 * blur * vstep)) * 0.0162162162;
}
