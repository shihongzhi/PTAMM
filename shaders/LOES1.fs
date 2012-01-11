uniform sampler2D txLOE;
uniform float ws;
uniform float hs;
uniform int loe_hs;

/// output: horizontal average of [fx*fx, fx*fy, fy*fy, 0.0]

void main() 
{
	vec4 currdirt = (texture2D( txLOE, gl_TexCoord[0].st ));
	if(loe_hs<=0 || currdirt.xy==vec2(0.0,0.0))
		gl_FragColor = currdirt;
	else
	{
		// smooth the jacobian matrix
		vec3 sum = vec3(0.0);
		for(int i=-loe_hs; i<=loe_hs; i++)
		{
			vec2 dirt = (texture2D(txLOE, gl_TexCoord[0].st + vec2(ws*float(i),0.0)	)).xy;
			sum += vec3(dirt.x*dirt.x, dirt.x*dirt.y, dirt.y*dirt.y);
		}
		sum = sum/float(2*loe_hs+1);
		vec4 res = vec4(sum,currdirt.z);
		gl_FragColor = res;
	}
	//gl_FragColor = texture2D(txLOE, gl_TexCoord[0].st);
}
