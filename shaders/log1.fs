uniform sampler2D txHSV;
uniform sampler2D txGauss;
uniform float ws;
uniform float hs;
uniform int iEnhance;
uniform int logSize;
uniform int display;

void main()
{
	if((display!=0 && display!=10) || iEnhance==0)
		gl_FragColor = texture2D(txHSV, gl_TexCoord[0].st);
	else
	{
		int neighbor = logSize*2+1;

		vec2 sum = vec2(0.0);
		for(int k=0; k<neighbor; k++)
		{
			float wet0 = texture2D(txGauss, vec2((float(k)+0.5)/float(neighbor),0.5/3.0)).x;
			sum.x += wet0 * texture2D( txHSV, gl_TexCoord[0].st + vec2(ws*float(k-logSize),0.0) ).z;
			sum.y += wet0 * texture2D( txHSV, gl_TexCoord[0].st + vec2(0.0,hs*float(k-logSize)) ).z;
		}
		gl_FragColor = vec4(sum,0.0,0.0);
	}
}