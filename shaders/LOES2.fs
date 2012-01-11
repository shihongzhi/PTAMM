uniform sampler2D txJaco;
uniform float ws;
uniform float hs;
uniform int loe_hs;
/// output: [normalized smoothed direction, fitting value(3), (4)]
/// (3)(4) is the same as the output of the 'g2' shader 

void main() 
{
	vec4 currdirt = (texture2D( txJaco, gl_TexCoord[0].st ));
	if(loe_hs<=0 || currdirt.xy==vec2(0.0,0.0))
		gl_FragColor = vec4(normalize(currdirt.xy),currdirt.zw);
	else
	{
		// smooth the jacobian matrix
		vec3 sum = vec3(0.0);
		for(int i=-loe_hs; i<=loe_hs; i++)
			sum += (texture2D(txJaco, gl_TexCoord[0].st + vec2(0.0,hs*float(i))	)).xyz;
		sum = sum/float(2*loe_hs+1);

		// get the new orientation
		float sqrtVal = sqrt( (sum.x-sum.z)*(sum.x-sum.z) + 4.0*sum.y*sum.y );
		vec2 lambda = vec2( (sum.x+sum.z + sqrtVal)/2.0, (sum.x+sum.z - sqrtVal)/2.0 );	
		vec2 newdirt = abs(sum.x)<abs(sum.z) ? vec2(sum.y, lambda.x-sum.x) : vec2( lambda.x - sum.z, sum.y);
		
		// get the new direction, according to the original direction
		float cosVal = newdirt.x*currdirt.x + newdirt.y*currdirt.y;
		newdirt = cosVal<0.0 ? -newdirt:newdirt;

		// get the new direction, preserving the magnitude
		vec4 res = vec4(normalize(newdirt),currdirt.zw);
		res.z = texture2D(txJaco, gl_TexCoord[0].st).w;
		gl_FragColor = res;
	}
	//gl_FragColor = texture2D(txJaco, gl_TexCoord[0].st);
}
