uniform sampler2D txGrad1;
uniform sampler2D txGauss;
uniform float ws;
uniform float hs;
uniform int smoothSize;

void main()
{
	int neighbor = smoothSize*2+1;

	vec2 grad = vec2(0.0);
	float coordOff = 0.5;

	for(int k=0; k<neighbor; k++)
	{
		float wet = texture2D(txGauss, vec2((float(k)+coordOff)/float(neighbor),1.0/3.0)).r;  // ??
		grad.x += wet * (texture2D( txGrad1, gl_TexCoord[0].st + vec2(ws*float(k-smoothSize),0.0) )).x;
		grad.y += wet * (texture2D( txGrad1, gl_TexCoord[0].st + vec2(0.0,hs*float(k-smoothSize)) )).y;
	}
	float len = length(grad)<0.0000001 ? 0.0:length(grad);
	grad = len==0.0 ? vec2(0.0):normalize(grad);

	gl_FragColor = vec4(grad,len,0.0);
}