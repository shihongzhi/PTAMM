
uniform sampler2D txLab;
uniform sampler2D txLOESmooth;
uniform sampler2D txMask;
uniform float ws;
uniform float hs;
uniform float sigma_e;
uniform float tau;

float sigma_d = 3.0;
float sige1 = 2.0*float(sigma_e)*float(sigma_e);
float sige2 = 2.0*float(sigma_e)*float(sigma_e)*1.6*1.6;

vec4 WeightColor( in vec2 pn0, in vec2 pn1, in float interpolate, in float weight_e1, in float weight_e2)
{
	vec2 sam0 = (texture2D(txMask,gl_TexCoord[0].st).x>0.0 || texture2D(txMask,gl_TexCoord[0].st+pn0).x>0.0) ? gl_TexCoord[0].st:gl_TexCoord[0].st+pn0;
	float floorClr = (texture2D(txLab, sam0 )).r;
	vec2 sam1 = (texture2D(txMask,gl_TexCoord[0].st).x>0.0 || texture2D(txMask,gl_TexCoord[0].st+pn1).x>0.0) ? gl_TexCoord[0].st:gl_TexCoord[0].st+pn1;
	float ceilClr = (texture2D(txLab, sam1 )).r;
	float interClr = interpolate*floorClr + (1.0-interpolate)*ceilClr;
	return vec4(weight_e1*interClr, weight_e1, weight_e2*interClr, weight_e2);
}
vec2 SamplingValue(in float val)
{
	float interpolate = (ceil(val)-val)==0.0? 1.0:(ceil(val)-val);
	interpolate = val<0.0 ? (val-floor(val)):interpolate;
	val += 0.5;
	return vec2(floor(val),interpolate);
}
float GradientBasedDoG()
{
	vec2 tang = (texture2D(txLOESmooth, gl_TexCoord[0].st)).xy;
	vec2 dirt = vec2(-tang.y, tang.x);

	float center = (texture2D(txLab, gl_TexCoord[0].st)).x;
	vec4 valsum = vec4(center,1.0,center,1.0);	// (val1,wit1,val2,wit2)
	
	if(dirt==vec2(0.0,0.0))
		return 0.0;
	else if( abs(dirt.x)>= abs(dirt.y) )		/// more horizontal
	{
		float stepy = dirt.y/dirt.x;
		float den1 = (1.0+stepy*stepy)/sige1;
		float den2 = (1.0+stepy*stepy)/sige2;
		int neighbor = int(floor(2.0*sigma_d/sqrt(1.0+stepy*stepy)));

		for(int i=1; i<=neighbor; i++)
		{
			float wight_e1 = exp( - float(i*i)*den1 );
			float wight_e2 = exp( - float(i*i)*den2 );
			vec2 sampling = SamplingValue(float(i)*stepy);	/// positive direction
			valsum += WeightColor(vec2(float(i)*ws,sampling.x*hs),vec2(float(i)*ws,(sampling.x+1.0)*hs),sampling.y,wight_e1,wight_e2);

			sampling = SamplingValue(-float(i)*stepy);		/// negative direction
			valsum += WeightColor(vec2(-float(i)*ws,sampling.x*hs),vec2(-float(i)*ws,(sampling.x+1.0)*hs),sampling.y,wight_e1,wight_e2);
		} 
	} 
	else		/// more vertical
	{
		float stepx = dirt.x/dirt.y;
		float den1 = (1.0+stepx*stepx)/sige1;
		float den2 = (1.0+stepx*stepx)/sige2;
		int neighbor = int(floor(2.0*sigma_d/sqrt(1.0+stepx*stepx)));

		for(int i=1; i<=neighbor; i++)
		{
			float wight_e1 = exp( - float(i*i)*den1 );
			float wight_e2 = exp( - float(i*i)*den2 );
			vec2 sampling = SamplingValue(float(i)*stepx);	/// positive direction 
			valsum += WeightColor(vec2(sampling.x*ws,float(i)*hs),vec2((sampling.x+1.0)*ws,float(i)*hs),sampling.y,wight_e1,wight_e2);

			sampling = SamplingValue(-float(i)*stepx);		/// negative direction
			valsum += WeightColor(vec2(sampling.x*ws,-float(i)*hs),vec2((sampling.x+1.0)*ws,-float(i)*hs),sampling.y,wight_e1,wight_e2);
		}
	} 
	if(valsum.y==0.0 || valsum.w==0.0)
		return 0.0;
	else 
		return valsum.x/valsum.y - tau*valsum.z/valsum.w;
}

void main()
{
	gl_FragColor = vec4(GradientBasedDoG());
}
