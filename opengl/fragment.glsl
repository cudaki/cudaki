uniform float time;
uniform int w;
uniform int h;

void main()
{
	vec2 pixel   = gl_TexCoord[0].st;
	gl_FragColor = vec4(pixel.y, 0.0, 0.0, 1.0);
}