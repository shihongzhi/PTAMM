uniform sampler2D txImage;
uniform sampler2D txGrad2;
uniform float ws;
uniform float hs;

int showType = 4;
float lowThres = 0.1;
float highThres = 0.2;

vec4 cannyLines()
{
	float low = lowThres<highThres?lowThres:highThres;
	float high= lowThres<highThres?highThres:lowThres;

	float mag = texture2D(txGrad2,gl_TexCoord[0].st).z*5.0;
	//float val = mag;															// raw value
	//float val = mag<=low?0.0: (mag>high?1.0:mag);								// clamp function
	//float val = mag<=low?0.0: (mag>=high?1.0:(mag-low)/(high-low));			// step function
	float val = smoothstep(low,high,mag);										// smoothstep function
	return vec4(1.0-val);
}

vec4 NonMaximumSuppression()
{
	vec2 dirt = texture2D(txGrad2,gl_TexCoord[0].st).xy;
	if(length(dirt)==0.0)
		return vec4(1.0);
	if(abs(dirt.x)>=abs(dirt.y))
	{
		float yoff = dirt.y/dirt.x;
		float magCen1 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(ws,0.0)).z;
		float magOff1 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(ws,yoff>0.0?hs:-hs)).z;
		float mag1 = (1.0-abs(yoff))*magCen1 + abs(yoff)*magOff1;

		float magCen2 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(-ws,0.0)).z;
		float magOff2 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(-ws,-yoff>0.0?hs:-hs)).z;
		float mag2 = (1.0-abs(yoff))*magCen2 + abs(yoff)*magOff2;

		float mag = texture2D(txGrad2,gl_TexCoord[0].st).z;
		if(mag<max(mag1,mag2))
			return vec4(1.0);
	}
	else
	{
		float xoff = dirt.x/dirt.y;
		float magCen1 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(0.0,hs)).z;
		float magOff1 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(xoff>0.0?ws:-ws,hs)).z;
		float mag1 = (1.0-abs(xoff))*magCen1 + abs(xoff)*magOff1;

		float magCen2 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(0.0,-hs)).z;
		float magOff2 = texture2D(txGrad2,gl_TexCoord[0].st+vec2(-xoff>0.0?ws:-ws,-hs)).z;
		float mag2 = (1.0-abs(xoff))*magCen2 + abs(xoff)*magOff2;

		float mag = texture2D(txGrad2,gl_TexCoord[0].st).z;
		if(mag<max(mag1,mag2))
			return vec4(1.0);
	}
	return cannyLines();
}

void main()
{
	if(showType==0)	/// original 
		gl_FragColor = texture2D(txImage,gl_TexCoord[0].st);
	else if(showType==1)	/// derivative - x
		gl_FragColor = vec4(texture2D(txGrad2,gl_TexCoord[0].st).x*0.5+0.5);
	else if(showType==2)	/// derivative - y
		gl_FragColor = vec4(texture2D(txGrad2,gl_TexCoord[0].st).y*0.5+0.5);
	else if(showType==3)	/// direction
		gl_FragColor = vec4(normalize(texture2D(txGrad2,gl_TexCoord[0].st).xy)*0.5+0.5, 0.0, 0.0);
	else if(showType==4)	/// magnitude
		gl_FragColor = vec4(texture2D(txGrad2,gl_TexCoord[0].st).z)*3.0;
	else if(showType==5)	/// canny
		//gl_FragColor = cannyLines();
		gl_FragColor = NonMaximumSuppression();
}
