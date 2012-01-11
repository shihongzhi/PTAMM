#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txGauss1;
uniform sampler2DRect txGauss2;
uniform sampler2DRect txBuff1;
uniform sampler2DRect txBuff2;
uniform sampler2DRect txImage;

uniform int iInside;    //1或0
uniform int tranSize1;  //5
uniform int tranSize2;  //8
uniform int iPass;      //第几次
uniform int iBilateral; //1
uniform float bfSig;    //0.2

float diffuseX()
{
	int tsz = iInside==1 ? tranSize1:tranSize2;

	float alpha = texture2DRect(txBuff1, gl_TexCoord[0].st).a;

	float step = 1.0;
	if(iInside==1)
	{
		if(alpha==0.0) return 0.0;

		if(iBilateral!=0)
		{
			float sigC = bfSig*bfSig*2.0;
			int neighbor = 2*tsz + 1;
			vec2 sum = vec2(0.0);
			vec3 currClr = texture2DRect(txImage, gl_TexCoord[0].st).rgb;
			for(int xi=-tsz; xi<=tsz; xi++)
			{
				vec2 samLoc = vec2((float(xi+tsz)+0.5)/float(neighbor), 0.5/3.0);

			}
		}
	}
}

void main(void) 
{
	gl_FragColor = vec4(diffuseX());
}