#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txBuff;


void main(void)
{
	vec2 curr = gl_TexCoord[0].st;
	vec4 object = texture2DRect(txBuff, curr);

	vec4 sum = vec4(0.0);

	sum += texture2DRect(txBuff, vec2(curr.x, curr.y-4.0)) * 0.05;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y-3.0)) * 0.09;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y-2.0)) * 0.12;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y-1.0)) * 0.15;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y)) * 0.16;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y+1.0)) * 0.15;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y+2.0)) * 0.12;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y+3.0)) * 0.09;
	sum += texture2DRect(txBuff, vec2(curr.x, curr.y+4.0)) * 0.05;

	gl_FragColor = sum;
}
