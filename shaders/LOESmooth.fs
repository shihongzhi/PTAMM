// work with a default vertex shader

uniform sampler2D txLOE;
uniform float ws;
uniform float hs;
uniform int loe_hs;

vec4 SmoothStructureTensor()
{
	float sig_gau = float(loe_hs)/3.0;
	float den = 2.0*sig_gau*sig_gau*1.0;
	
	int sig = int(float(loe_hs)/2.0);
	float ss = float( (2*sig+1)*(2*sig+1) );
	ss = 0.0;
	
	vec2 center = (texture2D( txLOE, gl_TexCoord[0].st )).xy;
	float len = length(center);
	
	vec3 sum = vec3(0.0, 0.0, 0.0);
	for(int yy=-sig; yy<=sig; yy++)
	{
		for(int xx=-sig; xx<=sig; xx++)
		{
			float gau = exp(-float(xx*xx+yy*yy)/den);
			vec2 tmp = (texture2D( txLOE, gl_TexCoord[0].st + vec2(float(xx)*ws, float(yy)*hs) )).xy;
			float wet =  gau;	//(dot(center,tmp)>=0.0?1.0:-1.0) *
			sum +=  vec3(tmp, 1.0) * wet;
		}
	}
	sum = sum/sum.z;
	float sumLen = length(sum.xy);
	if(sumLen==0.0)
		return vec4(0.0, 0.0, 0.0, 0.0);
	else
		return vec4(len*sum.x/sumLen, len*sum.y/sumLen, len, 0.0);
}

void main()
{	
	gl_FragColor = SmoothStructureTensor();
	//gl_FragColor = (texture2D( txLOE, gl_TexCoord[0].st ));
}
