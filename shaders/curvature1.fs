uniform sampler2D txLab;
uniform sampler2D txGauss;
uniform sampler2D txMask;
uniform float ws;
uniform float hs;
uniform int logSize;

void main()
{
	int neighbor = logSize*2+1;
	vec2 sum = vec2(0.0);
	for(int k=0; k<neighbor; k++)
	{
		float wet0 = texture2D(txGauss, vec2((float(k)+0.5)/float(neighbor),0.5/3.0)).x;
		sum.x += wet0 * texture2D( txLab, gl_TexCoord[0].st + vec2(ws*float(k-logSize),0.0) ).x;
		sum.y += wet0 * texture2D( txLab, gl_TexCoord[0].st + vec2(0.0,hs*float(k-logSize)) ).x;
	}
	gl_FragColor = vec4(sum,0.0,0.0);
}