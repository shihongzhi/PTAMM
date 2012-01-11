// work with a default vertex shader

uniform sampler2D txLab;
uniform sampler2D txLOESmooth;
uniform float ws;
uniform float hs;
uniform float sigma_d;
uniform float sigma_r;
uniform int bGrad;

float sigma_r2 = 2.0*sigma_r*sigma_r/(255.0*255.0);
float sigma_d2 = 2.0*float(sigma_d)*float(sigma_d);

vec4 WeightColor(in vec3 center, in vec2 pn0, in vec2 pn1, in float interpolate, in float weight_d)
{
	vec3 floorClr = (texture2D(txLab, gl_TexCoord[0].st + pn0 )).rgb;
	vec3 ceilClr = (texture2D(txLab, gl_TexCoord[0].st + pn1 )).rgb;
	vec3 interClr = interpolate*floorClr + (1.0-interpolate)*ceilClr;
	vec3 diffClr = interClr - center;
	float wight_r = weight_d * exp( -( dot(diffClr,diffClr) )/sigma_r2 );
	return vec4(wight_r*interClr, wight_r);
}
vec2 SamplingValue(in float val)
{
	float interpolate = (ceil(val)-val)==0.0? 1.0:(ceil(val)-val);
	interpolate = val<0.0 ? (val-floor(val)):interpolate;
	val += 0.5;
	return vec2(floor(val),interpolate);
}
vec4 FlowBasedBilateralfilter()
{
	vec2 tang = (texture2D(txLOESmooth, gl_TexCoord[0].st)).xy;
	vec2 dirt = bGrad==0 ? tang:vec2(-tang.y, tang.x);

	vec3 center = (texture2D(txLab, gl_TexCoord[0].st)).rgb;
	vec4 valsum = vec4(center,1.0);
	
	if(dirt==vec2(0.0,0.0))
		return valsum;
	else if( abs(dirt.x)>= abs(dirt.y) )		/// more horizontal
	{
		float stepy = dirt.y/dirt.x;
		float den = (1.0+stepy*stepy)/sigma_d2;
		int neighbor = int(floor(2.0*sigma_d/sqrt(1.0+stepy*stepy)));

		for(int i=1; i<=neighbor; i++)
		{
			float wight_d = exp( - float(i*i)*den );
			vec2 sampling = SamplingValue(float(i)*stepy);	/// positive direction
			valsum += WeightColor(center,vec2(float(i)*ws,sampling.x*hs),vec2(float(i)*ws,(sampling.x+1.0)*hs),sampling.y,wight_d);

			sampling = SamplingValue(-float(i)*stepy);		/// negative direction
			valsum += WeightColor(center,vec2(-float(i)*ws,sampling.x*hs),vec2(-float(i)*ws,(sampling.x+1.0)*hs),sampling.y,wight_d);
		} 
	} 
	else		/// more vertical
	{
		float stepx = dirt.x/dirt.y;
		float den = (1.0+stepx*stepx)/sigma_d2;
		int neighbor = int(floor(2.0*sigma_d/sqrt(1.0+stepx*stepx)));

		for(int i=1; i<=neighbor; i++)
		{
			float wight_d = exp( - float(i*i)*den );
			vec2 sampling = SamplingValue(float(i)*stepx);	/// positive direction 
			valsum += WeightColor(center,vec2(sampling.x*ws,float(i)*hs),vec2((sampling.x+1.0)*ws,float(i)*hs),sampling.y,wight_d);

			sampling = SamplingValue(-float(i)*stepx);		/// negative direction
			valsum += WeightColor(center,vec2(sampling.x*ws,-float(i)*hs),vec2((sampling.x+1.0)*ws,-float(i)*hs),sampling.y,wight_d);
		}
	} 
	return vec4(valsum.xyz/valsum.w, 1.0);
}

void main()
{
	gl_FragColor = FlowBasedBilateralfilter();
}
