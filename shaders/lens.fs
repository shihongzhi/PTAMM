uniform int width;
uniform int height;
uniform int mousex;
uniform int mousey;
uniform int iLens;
uniform int lensSize;

void main(void) 
{
	if(iLens==0)  gl_FragColor = vec4(1.0);
	else
	{
		float dismax = float(lensSize);
		float dismin = dismax/3.0;
		float valmin = 0.0;
		float valmax = 1.0;
		
		vec2 pos = gl_TexCoord[0].st*vec2(float(width),float(height));
		pos = pos - vec2(float(mousex),float(mousey));
		//float dis = clamp( (length(pos)-dismin)/(dismax-dismin), 0.0, 1.0);
		float dis = smoothstep(0.0, 1.0, (length(pos)-dismin)/(dismax-dismin) );

		gl_FragColor = vec4(dis*valmin + (1.0-dis)*valmax);
	}
}
