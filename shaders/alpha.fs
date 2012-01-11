#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txBuff;


void main(void)
{
	vec2 curr = gl_TexCoord[0].st;
	vec4 object = texture2DRect(txBuff, curr);

	if(vec3(object) == vec3(0.0))
		gl_FragColor = vec4(vec3(0.0), 1.0);
	else
		gl_FragColor = vec4(vec3(1.0), 1.0);
}
