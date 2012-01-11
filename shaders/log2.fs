uniform sampler2D txBuff1;
uniform sampler2D txLoG1;
uniform sampler2D txEdge;
uniform sampler2D txGauss;
uniform sampler2D txBlend;
uniform sampler2D txGS2;
uniform sampler2D txHSV;
uniform sampler2D txUnder;
uniform sampler2D txTran2;
uniform sampler2D txCurv;
uniform sampler2D txMask;

uniform float ws;
uniform float hs;

uniform int iCutPlane;
uniform float cutPlane;
uniform int iEnhance;
uniform int logSize;
uniform int enhanceSize;
uniform float enhancePro;

uniform int display;

float hori = cutPlane;
float band = 0.005;

vec3 HSV2RGB(vec3 hsv)
{
	if(hsv.y==0.0)
		return vec3(hsv.z);

	float c = hsv.y*hsv.z;
	float hi = hsv.x*520.0/60.0;
	float mod = hi;
	while(mod>=2.0)
		mod -= 2.0;
	float x = c * (1.0 - abs(mod-1.0));
	
	vec3 rgb;
	if(hi>=0.0 && hi<1.0)		rgb = vec3(c, x, 0.0);
	else if(hi>=1.0 && hi<2.0)	rgb = vec3(x, c, 0.0);
	else if(hi>=2.0 && hi<3.0)  rgb = vec3(0.0, c, x);
	else if(hi>=3.0 && hi<4.0)	rgb = vec3(0.0, x, c);
	else if(hi>=4.0 && hi<5.0)	rgb = vec3(x, 0.0, c);
	else if(hi>=5.0 && hi<6.0)	rgb = vec3(c, 0.0, x);
	else		rgb = vec3(0.0, 0.0, 0.0);
		
	float m = hsv.z - c;
	return rgb+m;
}

float enhanceValue()
{
	int neighbor = logSize*2+1;
	float sum = 0.0;
	for(int k=0; k<neighbor; k++)
	{
		float wet1 = texture2D(txGauss, vec2((float(k)+0.5)/float(neighbor),2.5/3.0)).r;
		sum += wet1 * (texture2D( txLoG1, gl_TexCoord[0].st + vec2(0.0,hs*float(k-logSize)) ).y)/2.0;
		sum += wet1 * (texture2D( txLoG1, gl_TexCoord[0].st + vec2(ws*float(k-logSize),0.0) ).x)/2.0;
	}
	return sum;
}

float mask_region()
{
	int rad=2;
	for(int yi=-rad; yi<=rad; yi++)
	{
		for(int xi=-rad; xi<=rad; xi++)
		{
			vec2 loc = gl_TexCoord[0].st + vec2(float(xi)*ws, float(yi)*hs);
			if( texture2D(txMask, loc).x >=0.5 )
				return 0.0;
		}
	}
	return 1.0;
}

vec4 enhancement()
{	
	////// underground texutre
	vec2 curr = gl_TexCoord[0].st;
	vec4 und = texture2D(txUnder, curr);
	if(iCutPlane==1)
	{
		if(curr.y>=hori-band && curr.y<=hori+band)	
			return vec4(0.0,0.0,0.0,0.0);
		else if(curr.y<hori-band)
		{
			vec4 render = texture2D(txBuff1, gl_TexCoord[0].st);
			return mix(und, render, render.a);
		}
	}

	if(iEnhance==0)
		return texture2D(txBlend, gl_TexCoord[0].st);

	//// compute the original LoG value
	float enhVal = -enhanceValue();	

	//// cancel the dynamic region
	enhVal *= mask_region();

	//// combined with transition_outside
	enhVal *= texture2D(txTran2, gl_TexCoord[0].st).x;
	//return vec4(enhVal*0.5*5.0+0.5);	

	// only enhace around the image lines
	float adpt = pow(float(enhanceSize),1.0/5.0);
	float lenh = clamp((1.0-texture2D(txGS2, gl_TexCoord[0].st).x)*adpt, 0.0, 1.0);
	//return vec4(lenh*texture2D(txTran2, gl_TexCoord[0].st).x);

	////==================================== improved enhancment
	vec4 currClr = texture2D(txBlend,gl_TexCoord[0].st);
	float gray = exp(texture2D(txHSV,gl_TexCoord[0].st).z );
	
	float lineEnh = enhVal * 30.0 * enhancePro * lenh;
	//return vec4(lineEnh+0.5);

	if(display!=0)
		return vec4(clamp(lineEnh*0.5 + 0.5, 0.0, 1.0));
	else
	{
		vec3 hsv = texture2D(txHSV,gl_TexCoord[0].st).rgb + vec3( 0.0, 0.0, lineEnh);
		return vec4( HSV2RGB(hsv), 0.0 );
	}
}

void main()
{
	if((display!=0 && display!=10))
		gl_FragColor = clamp(texture2D(txBlend, gl_TexCoord[0].st),0.0,1.0);
	else
		gl_FragColor = clamp(enhancement(),0.0,1.0);
		
}