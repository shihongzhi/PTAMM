#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txBuff;


void main(void)
{
	vec2 curr = gl_TexCoord[0].st;
	vec4 object = texture2DRect(txBuff, curr);

	vec4 sum = vec4(0.0);

	sum += texture2DRect(txBuff, curr+vec2(-3.0, -3.0)) * 0.00000067;
	sum += texture2DRect(txBuff, curr+vec2(-2.0, -3.0)) * 0.00002292;
	sum += texture2DRect(txBuff, curr+vec2(-1.0, -3.0)) * 0.00019117;
	sum += texture2DRect(txBuff, curr+vec2(0.0, -3.0)) * 0.00038771;
	sum += texture2DRect(txBuff, curr+vec2(1.0, -3.0)) * 0.00019117;
	sum += texture2DRect(txBuff, curr+vec2(2.0, -3.0)) * 0.00002292;
	sum += texture2DRect(txBuff, curr+vec2(3.0, -3.0)) * 0.00000067;

	sum += texture2DRect(txBuff, curr+vec2(-3.0, -2.0)) * 0.00002292;
	sum += texture2DRect(txBuff, curr+vec2(-2.0, -2.0)) * 0.00078633;
	sum += texture2DRect(txBuff, curr+vec2(-1.0, -2.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(0.0, -2.0)) * 0.01330373;
	sum += texture2DRect(txBuff, curr+vec2(1.0, -2.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(2.0, -2.0)) * 0.00078633;
	sum += texture2DRect(txBuff, curr+vec2(3.0, -2.0)) * 0.00002292;

	sum += texture2DRect(txBuff, curr+vec2(-3.0, -1.0)) * 0.00019117;
	sum += texture2DRect(txBuff, curr+vec2(-2.0, -1.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(-1.0, -1.0)) * 0.05472157;
	sum += texture2DRect(txBuff, curr+vec2(0.0, -1.0)) * 0.11098164;
	sum += texture2DRect(txBuff, curr+vec2(1.0, -1.0)) * 0.05472157;
	sum += texture2DRect(txBuff, curr+vec2(2.0, -1.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(3.0, -1.0)) * 0.00019117;

	sum += texture2DRect(txBuff, curr+vec2(-3.0, 0.0)) * 0.00038771;
	sum += texture2DRect(txBuff, curr+vec2(-2.0, 0.0)) * 0.01330373;
	sum += texture2DRect(txBuff, curr+vec2(-1.0, 0.0)) * 0.11098164;
	sum += texture2DRect(txBuff, curr+vec2(0.0, 0.0)) * 0.22508352;
	sum += texture2DRect(txBuff, curr+vec2(1.0, 0.0)) * 0.11098164;
	sum += texture2DRect(txBuff, curr+vec2(2.0, 0.0)) * 0.01330373;
	sum += texture2DRect(txBuff, curr+vec2(3.0, 0.0)) * 0.00038771;

	sum += texture2DRect(txBuff, curr+vec2(-3.0, 1.0)) * 0.00019117;
	sum += texture2DRect(txBuff, curr+vec2(-2.0, 1.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(-1.0, 1.0)) * 0.05472157;
	sum += texture2DRect(txBuff, curr+vec2(0.0, 1.0)) * 0.11098164;
	sum += texture2DRect(txBuff, curr+vec2(1.0, 1.0)) * 0.05472157;
	sum += texture2DRect(txBuff, curr+vec2(2.0, 1.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(3.0, 1.0)) * 0.00019117;

	sum += texture2DRect(txBuff, curr+vec2(-3.0, 2.0)) * 0.00002292;
	sum += texture2DRect(txBuff, curr+vec2(-2.0, 2.0)) * 0.00078633;
	sum += texture2DRect(txBuff, curr+vec2(-1.0, 2.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(0.0, 2.0)) * 0.01330373;
	sum += texture2DRect(txBuff, curr+vec2(1.0, 2.0)) * 0.00655965;
	sum += texture2DRect(txBuff, curr+vec2(2.0, 2.0)) * 0.00078633;
	sum += texture2DRect(txBuff, curr+vec2(3.0, 2.0)) * 0.00002292;

	sum += texture2DRect(txBuff, curr+vec2(-3.0, 3.0)) * 0.00000067;
	sum += texture2DRect(txBuff, curr+vec2(-2.0, 3.0)) * 0.00002292;
	sum += texture2DRect(txBuff, curr+vec2(-1.0, 3.0)) * 0.00019117;
	sum += texture2DRect(txBuff, curr+vec2(0.0, 3.0)) * 0.00038771;
	sum += texture2DRect(txBuff, curr+vec2(1.0, 3.0)) * 0.00019117;
	sum += texture2DRect(txBuff, curr+vec2(2.0, 3.0)) * 0.00002292;
	sum += texture2DRect(txBuff, curr+vec2(3.0, 3.0)) * 0.00000067;

	gl_FragColor = sum;
}
