uniform sampler2D txDiff;


void main(void) 
{
	vec4 clr = texture2D(txDiff, gl_TexCoord[0].st);
	if(clr.a!=0.0)
		gl_FragColor = clr;
	else
		gl_FragColor = vec4(1.0,1.0,1.0,0.0);

}
