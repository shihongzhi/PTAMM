// work with a default vertex shader

uniform sampler2D txImage;
uniform float ws;
uniform float hs;

vec4 LocalOrientationEstimation()
{
	vec3 clr00 = (texture2D(txImage, gl_TexCoord[0].st + vec2(-ws,	-hs	))).rgb;
	vec3 clr10 = (texture2D(txImage, gl_TexCoord[0].st + vec2(0,	-hs	))).rgb;
	vec3 clr20 = (texture2D(txImage, gl_TexCoord[0].st + vec2( ws,	-hs	))).rgb;
	vec3 clr01 = (texture2D(txImage, gl_TexCoord[0].st + vec2(-ws, 	0	))).rgb;
	vec3 clr21 = (texture2D(txImage, gl_TexCoord[0].st + vec2( ws, 	0	))).rgb;
	vec3 clr02 = (texture2D(txImage, gl_TexCoord[0].st + vec2(-ws, 	hs 	))).rgb;
	vec3 clr12 = (texture2D(txImage, gl_TexCoord[0].st + vec2(0,	hs 	))).rgb;
	vec3 clr22 = (texture2D(txImage, gl_TexCoord[0].st + vec2( ws, 	hs 	))).rgb;
	
	vec3 fx = (clr20-clr00 + clr22-clr02 + 2.0*(clr21-clr01))*255.0;
	vec3 fy = (clr02-clr00 + clr22-clr20 + 2.0*(clr12-clr10))*255.0;

//	vec2 gra = vec2(fx.x+fx.y+fx.z, fy.x+fy.y+fy.z);
//	return vec4(-gra.y, gra.x, 0.0, 0.0);
	
	if(true)
	{
		vec3 rgb2lumi = vec3(0.299, 0.587, 0.114);
		vec2 tang = vec2(-dot(fy,rgb2lumi),dot(fx,rgb2lumi));
		//return vec4(tang,0.0,0.0);
		return vec4(tang,length(tang),0.0);
	}
	else
	{
		vec3 jaco = vec3( dot(fx,fx), dot(fx,fy), dot(fy,fy) ); 

		float sqrtVal = sqrt( (jaco.x-jaco.z)*(jaco.x-jaco.z) + 4.0*jaco.y*jaco.y );
		vec2 lambda = vec2( (jaco.x+jaco.z + sqrtVal)/2.0,(jaco.x+jaco.z - sqrtVal)/2.0 );
		vec2 tang = abs(jaco.x)<abs(jaco.z) ? vec2(jaco.y, lambda.x-jaco.x) : vec2(lambda.x-jaco.z, jaco.y);
		
		return vec4(tang,length(tang),0.0);
	}
}

void main()
{
	gl_FragColor = LocalOrientationEstimation();
}
