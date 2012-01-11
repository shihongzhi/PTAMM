#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txTranX;
uniform sampler2DRect txImage;
uniform sampler2D txGauss1;
uniform sampler2D txGauss2;
uniform int iInside;

float diffuseY()
{
	vec2 curr = gl_TexCoord[0].st;
	int tsz = 5;
	float alpha = texture2DRect(txTranX, curr).a;
	float step = 1.0;
	
	if(iInside == 1)
	{
		if(alpha == 0.0) return 0.0;
	}
	else
	{
		step = 2.0;
		if(alpha == 1.0) return 1.0;
	}

	float sum = 0.0;
	int neighbor = 2*tsz + 1;
	for(int yi=0; yi<neighbor; yi++)
	{
		vec2 samLoc = vec2(float(yi)/float(neighbor), 0.0/1.0);
		float wet0 = texture2D(txGauss1, samLoc).r;
		vec2 imgLoc = curr + vec2(0.0, float(yi)*step);
		float alp = texture2DRect(txTranX, imgLoc).a;
		sum += wet0 * alp;
	}
	return sum;
}

void main(void)
{
	gl_FragColor = vec4(diffuseY());
}
