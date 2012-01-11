
uniform sampler2D txLab;
uniform sampler2D txDoG;
uniform sampler2D txLOESmooth;
uniform sampler2D txMask;
uniform float ws;
uniform float hs;
uniform float sigma_m;
uniform float phi;
uniform int iLast;

vec2 _get_move_direction( vec2 pos, vec2 prevDirect)
{
	vec2 tangent = texture2D(txLOESmooth, gl_TexCoord[0].st+vec2(pos.x*ws,pos.y*hs)).xy;

	float _cos = tangent.x*prevDirect.x+tangent.y*prevDirect.y;
	float _sin = tangent.x*prevDirect.y-tangent.y*prevDirect.x;

	if( _cos > 0.0 || ( _cos == 0.0 && _sin>0.0 ) )
		return tangent;
	else
		return -tangent;
}

vec2 _get_move_length( vec2 pos,  vec2 direct)
{
	if(direct==vec2(0.0,0.0))
		return vec2(0.0,0.0);
	else if( abs(direct.x) >= abs(direct.y) )
	{	
		if(direct.x>0.0)
		{
			float newx = ceil(pos.x);
			float lenx = (newx-pos.x)==0.0?1.0:(newx-pos.x);
			return vec2(lenx, lenx*direct.y/direct.x);
		}
		else
		{
			float newx = floor(pos.x);
			float lenx = (newx-pos.x)==0.0?-1.0:(newx-pos.x);
			return vec2(lenx, lenx*direct.y/direct.x);
		}
	}
	else
	{
		if(direct.y>0.0)
		{
			float newy = ceil(pos.y);
			float leny = (newy-pos.y)==0.0?1.0:(newy-pos.y);
			return vec2(leny*direct.x/direct.y,leny);
		}
		else
		{
			float newy = floor(pos.y);
			float leny = (newy-pos.y)==0.0?-1.0:(newy-pos.y);
			return vec2(leny*direct.x/direct.y,leny);
		}
	}
}
vec2 _add_sample( vec2 samplePos, float expVal,  vec2 prevMove)	
{
	float gau = exp( -expVal );
	if(prevMove==vec2(0.0,0.0))
	{
		float lumi = texture2D(txDoG, gl_TexCoord[0].st+vec2(samplePos.x*ws,samplePos.y*hs)).x;
		return vec2(gau*lumi,gau);
	}
	else if( abs(prevMove.x)>=abs(prevMove.y) ) // more horizontally
	{
		float ipyF=floor(samplePos.y);
		float ipyC=ceil(samplePos.y);
		float interpolate = samplePos.y-ipyF;
		float lumiC = texture2D(txDoG, gl_TexCoord[0].st+vec2(samplePos.x*ws,ipyC*hs)).x;
		float lumiF = texture2D(txDoG, gl_TexCoord[0].st+vec2(samplePos.x*ws,ipyF*hs)).x;
		float lumi = lumiC*interpolate +lumiF*(1.0-interpolate);
		return vec2(gau*lumi, gau);
	}
	else // more vertically
	{
		float ipxF=floor(samplePos.x);
		float ipxC=ceil(samplePos.y);
		float interpolate = samplePos.x-ipxF;
		float lumiC = texture2D(txDoG, gl_TexCoord[0].st+vec2(ipxC*ws,samplePos.y*hs)).x;
		float lumiF = texture2D(txDoG, gl_TexCoord[0].st+vec2(ipxF*ws,samplePos.y*hs)).x;
		float lumi = lumiC*interpolate + lumiF*(1.0-interpolate);
		return vec2(gau*lumi, gau);
	}
}
vec4 TangentBasedSmooth()
{
	float gauRadius = 3.0*sigma_m;
	float den_sigma_m = 2.0*sigma_m*sigma_m/7.0;
	
	vec2 sum = vec2(0.0, 0.0);

	float moveLength = 0.0;
	vec2 samplePos = vec2(0.0,0.0);
	vec2 moveDirect = _get_move_direction(samplePos,vec2(1.0,0.0));
	sum += _add_sample(samplePos,moveLength*moveLength,vec2(0.0,0.0));
	int cnt =0;
	while(cnt<=int(gauRadius+0.5))
	{
		vec2 move_offset = _get_move_length(samplePos,moveDirect);
		moveLength += length(move_offset);
		if( moveLength > gauRadius )
			break;
		samplePos += move_offset;

		sum += _add_sample(samplePos,moveLength*moveLength/den_sigma_m,moveDirect);
		moveDirect = _get_move_direction(samplePos,move_offset);

		cnt+=1;
	}
	
	moveLength = 0.0;
	samplePos = vec2(0.0,0.0);
	moveDirect = _get_move_direction(samplePos,vec2(-1.0,0.0));
	cnt = 0;
	while(cnt<=int(gauRadius+0.5))
	{
		vec2 move_offset = _get_move_length(samplePos,moveDirect);
		moveLength += length(move_offset);
		if( moveLength > gauRadius )
			break;
		samplePos += move_offset;

		sum += _add_sample(samplePos,moveLength*moveLength/den_sigma_m,moveDirect);
		moveDirect = _get_move_direction(samplePos,move_offset);

		cnt+=1;
	}

	float cond = sum.x/sum.y;
	float alpha = 1.0;
	if( cond<0.0 )
		alpha = (1.0+tanh(phi*cond*255.0));	
	
	return vec4(vec3(clamp(alpha,0.0,1.0)), -cond*100.0);
}

void main()
{
	
	vec4 res = TangentBasedSmooth();

	if(iLast==1)
		gl_FragColor = res;
	else	
		gl_FragColor = vec4(texture2D(txLab,gl_TexCoord[0].st).x * clamp(res.x,0.0,1.0));

	//gl_FragColor = texture2D(txDoG, gl_TexCoord[0].st)*200.0;
}
