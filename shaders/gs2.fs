uniform sampler2D txEdge;
uniform sampler2D txGauss;
uniform float ws;
uniform float hs;
uniform int enhanceSize;
uniform int display;

void main()
{
	if((display!=0 && display!=12))
		gl_FragColor = texture2D(txEdge, gl_TexCoord[0].st);
	else
	{
		int neighbor = enhanceSize*2+1;
		vec4 sum = vec4(0.0);
		for(int k=0; k<neighbor; k++)
		{
			float wet1 = texture2D(txGauss, vec2((float(k)+0.5)/float(neighbor),0.5/3.0)).r;
			sum += wet1 * texture2D( txEdge, gl_TexCoord[0].st + vec2(0.0,hs*float(k-enhanceSize)) );
		//	sum += wet1 * texture2D( txEdge, gl_TexCoord[0].st + vec2(ws*float(k-enhanceSize),0.0) )/2.0;
		}
		gl_FragColor = sum;
	}
}