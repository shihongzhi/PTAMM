uniform sampler2D txImage;
uniform sampler2D txGauss;
uniform float ws;
uniform float hs;
uniform int smoothSize;

void main()
{
	vec3 rgb2lumi = vec3(0.299, 0.587, 0.114);

	int neighbor = smoothSize*2+1;
	float coordOff = 0.5;

	vec4 sum = vec4(0.0);
	for(int k=0; k<neighbor; k++)
	{
		float wet0 = texture2D(txGauss, vec2((float(k)+coordOff)/float(neighbor),0.5)).r;  // ??
		sum.x += wet0 * dot(rgb2lumi,(texture2D( txImage, gl_TexCoord[0].st + vec2(0.0,hs*float(k-smoothSize)) )).rgb);
		sum.y += wet0 * dot(rgb2lumi,(texture2D( txImage, gl_TexCoord[0].st + vec2(ws*float(k-smoothSize),0.0) )).rgb);
	}
	gl_FragColor = sum;
}