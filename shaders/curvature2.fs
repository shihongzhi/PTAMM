uniform sampler2D txCurv1;
uniform sampler2D txGauss;
uniform sampler2D txMask;
uniform float ws;
uniform float hs;
uniform int logSize;

float curvature2()
{
	int neighbor = logSize*2+1;
	float sum = 0.0;
	vec2 loc;
	if(texture2D(txMask,gl_TexCoord[0].st).x>0.5 )
		return 0.0;

	for(int k=0; k<neighbor; k++)
	{
		float wet1 = texture2D(txGauss, vec2((float(k)+0.5)/float(neighbor),2.5/3.0)).x;
		
		loc = gl_TexCoord[0].st +vec2(0.0,hs*float(k-logSize));
		if(texture2D(txMask,loc).x>0.5)
			loc = gl_TexCoord[0].st;
		sum += wet1 * abs(texture2D( txCurv1,  loc ).y)/2.0;

		loc = gl_TexCoord[0].st +vec2(ws*float(k-logSize),0.0);
		if(texture2D(txMask,loc).x>0.5)
			loc = gl_TexCoord[0].st;
		sum += wet1 * abs(texture2D( txCurv1,  loc ).x)/2.0;
	}
	return sum;
}

void main()
{
	gl_FragColor = vec4(curvature2());
}