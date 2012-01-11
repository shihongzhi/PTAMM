uniform sampler2D txPhong;
uniform float ws;
uniform float hs;
uniform int cutAway;

vec4 simple()
{
	vec4 clr = texture2D(txPhong, gl_TexCoord[0].st);
	if(clr.a!=0.0)
		return vec4(1.0);
	else
		return vec4(0.0);
}

vec4 cutaway()
{
	int rad = cutAway;
	float alpha = texture2D(txPhong, gl_TexCoord[0].st).a;
	//if(alpha != 0.0)	return vec4(1.0);		/// external diffusion
	if(alpha == 0.0)	return vec4(0.0);		/// internal diffusion

	float aa = 0.0;
	float cnt = 0.0;
	for(int yi=-rad; yi<=rad; yi++)
	{
		for(int xi=-rad; xi<=rad; xi++)
		{
			if(xi*xi+yi*yi<=rad*rad)
			{
				aa += texture2D(txPhong, gl_TexCoord[0].st+vec2(float(xi)*ws,float(yi)*hs)).a;
				cnt += 1.0;
				//if(texture2D(txPhong, gl_TexCoord[0].st+vec2(float(xi)*ws,float(yi)*hs)).a != 0.0)
				//	return vec4(1.0);
			}
		}
	}
	return vec4(clamp(aa/cnt,0.0,1.0));
	//return vec4(0.0);
}

void main(void) 
{
	//gl_FragColor = simple();

	gl_FragColor = cutaway();

}