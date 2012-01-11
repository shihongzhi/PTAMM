#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txBuff1;
uniform sampler2DRect txBuff3;
uniform sampler2DRect txImage;
uniform sampler2D txGauss1;
uniform sampler2D txGauss2;
//uniform int iPass;
uniform int iInside;

float diffuseX()
{
	vec2 curr = gl_TexCoord[0].st;
	int tsz = 5;
	float alpha = texture2DRect(txBuff1, curr).a;
	int temp;
	//if(iPass==0)
	//	temp = 1;
		//alpha = alpha<1.0 ? texture2DRect(txBuff3, curr).a : 0.0;
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
	for(int xi=0; xi<neighbor; xi++)
	{
		vec2 samLoc = vec2(float(xi)/float(neighbor), 0.0/1.0);
		float wet0 = texture2D(txGauss1, samLoc).r;
		vec2 imgLoc = curr + vec2(float(xi)*step,0.0);
		float alp = texture2DRect(txBuff1, imgLoc).a;
		//if(iPass==0) 
		//	temp = 1;
			//alp = alp<1.0 ? texture2DRect(txBuff3, imgLoc).a : 0.0;
		sum += wet0 * alp;
	}
	return sum;
}

void main(void)
{
	gl_FragColor = vec4(diffuseX());
}
