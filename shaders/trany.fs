uniform sampler2D txGauss1;
uniform sampler2D txGauss2;
uniform sampler2D txTranX;
uniform sampler2D txImage;
uniform sampler2D txMask;
uniform float ws;
uniform float hs;
uniform int iInside;
uniform int tranSize1;
uniform int tranSize2;
uniform int iBilateral;
uniform float bfSig;

float color_dist(vec2 curr, vec2 off)
{
	if(texture2D(txMask, curr).x>0.5 || texture2D(txMask, off).x>0.5)
		return 0.0;
	else
		return length(texture2D(txImage, curr).rgb - texture2D(txImage, off).rgb);
}

float diffuseY()
{
	int tsz = iInside==1 ? tranSize1:tranSize2;
	float alpha = texture2D(txTranX, gl_TexCoord[0].st).a;
	
	if(tsz<=1)
				return texture2D(txTranX, gl_TexCoord[0].st).a;
	float step = 1.0;
	if(iInside==1) 
	{
		if(alpha == 0.0)	return 0.0;

		if(iBilateral!=0)		// bilateral filter inside transition
		{
			float sigC = bfSig*bfSig*2.0;
			int neighbor = 2*tsz + 1;
			vec2 sum = vec2(0.0);
			vec3 currClr = texture2D(txImage,gl_TexCoord[0].st).rgb;
			for(int yi=-tsz; yi<=tsz; yi++)
			{
				vec2 samLoc = vec2((float(yi+tsz)+0.5)/float(neighbor),0.5/3.0);
				float wet0 = iInside==1 ? texture2D(txGauss1,samLoc).x:texture2D(txGauss2,samLoc).x;
				vec2 imgLoc = gl_TexCoord[0].st+vec2(0.0,float(yi)*hs*step);
				float alp = texture2D(txTranX, imgLoc).a;
				float wetC = color_dist(gl_TexCoord[0].st, imgLoc);
				sum += vec2(alp,1.0) * wet0 * exp(-wetC*wetC/sigC);
			}
			if(sum.y==0.0)
				return texture2D(txTranX,gl_TexCoord[0].st).a;
			else
				return sum.x/sum.y;
		}
	}
	else {
		step = 2.0;
		if(alpha == 1.0)	return 1.0; }

	int neighbor = 2*tsz + 1;
	float sum = 0.0;
	for(int yi=-tsz; yi<=tsz; yi++)
	{
		vec2 samLoc = vec2((float(yi+tsz)+0.5)/float(neighbor),0.0/3.0);
		float wet0 = iInside==1 ? texture2D(txGauss1, samLoc).x:texture2D(txGauss2, samLoc).x;
		sum += wet0 * texture2D(txTranX, gl_TexCoord[0].st+vec2(0.0,float(yi)*hs*step)).a;
	}
	return sum;
}

void main(void) 
{
	gl_FragColor = vec4(diffuseY());
	//gl_FragColor = texture2D(txTranX, gl_TexCoord[0].st);
}