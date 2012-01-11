uniform sampler2D txBuff1;
uniform sampler2D txBuff2;
uniform sampler2D txBuff3;
uniform sampler2D txTran1;
uniform sampler2D txTran2;
uniform sampler2D txLine3D1;
uniform sampler2D txLine3D2;
uniform sampler2D txImage;

uniform sampler2D txEdge;


uniform sampler2D txNoise;
uniform sampler2D txMask;

uniform float ws;
uniform float hs;
uniform float beta;
uniform float alphaPro;

uniform int iLens;
uniform int display;

int licLen = 10;

float dotted_line3d()
{
	float line1 = texture2D(txLine3D1, gl_TexCoord[0].st).x;
	float line2 = texture2D(txLine3D2, gl_TexCoord[0].st).x;

	return line1*line2;

	// provide dotted 3d lines in the back face regions
	vec2 curr = gl_TexCoord[0].st;
	curr = vec2(curr.x/ws, curr.t/hs);
	//float grid = 1.0;
	float grid = step(0.0, (int(curr.x/8.0)*2==int(curr.x/4.0)?1.0:-1.0) * (int(curr.y/8.0)*2==int(curr.y/4.0)?1.0:-1.0) );
	grid = texture2D(txBuff1, gl_TexCoord[0].st).a - grid*texture2D(txBuff3, gl_TexCoord[0].st).a;
	return (1.0-(1.0-line1) * grid)*line2;
	//grid =  grid*texture2D(txBuff3, gl_TexCoord[0].st).a;
	//return (1.0-(1.0-line1) * grid);
}

float selected_line3d()
{
	// keep the 3d lines around the back face regions
	//for(int yi=-1; yi<=1; yi++)
	//{
	//	for(int xi=-1; xi<=1; xi++)
	//	{
	//		vec2 samLoc = gl_TexCoord[0].st+vec2(float(xi)*ws, float(yi)*hs);
	//		if( texture2D(txBuff3, samLoc).a >=1.0 )
	//			return dotted_line3d();
	//	}
	//}

//	return dotted_line3d();

	// remove the 3d lines around virtual objects' region (not including the back face regions)
	float alpK = step(1.0,texture2D(txBuff1, gl_TexCoord[0].st).a-texture2D(txBuff3, gl_TexCoord[0].st).a);
	for(int yi=-1; yi<=1; yi++)
	{
		for(int xi=-1; xi<=1; xi++)
		{
			vec2 samLoc = gl_TexCoord[0].st+vec2(float(xi)*ws, float(yi)*hs);
			float alp = step(1.0,texture2D(txBuff1, samLoc).a-texture2D(txBuff3, samLoc).a);
			if(alp!=alpK)
				return 1.0;
		}
	}
	return dotted_line3d();
}

float transition_inside_curvature()
{
	vec2 curr = gl_TexCoord[0].st;
	vec4 rendW = texture2D(txBuff1, curr);
	vec4 rendWo = texture2D(txBuff3, curr);			// rendered image without back culling
	float tranIn = texture2D(txTran1, curr).x;		// transition inside
	float curv = texture2D(txCurv, curr).x;

	float clrAlp = tranIn + rendWo.a*alphaPro;
	float intensity = abs(curv*250.0);
	//float intensity = dot(img.rgb, img.rgb);
	return pow(clrAlp,1.0 + beta*intensity);
}

vec4 blending()
{	
	vec2 curr = gl_TexCoord[0].st;
	vec4 rendW = texture2D(txBuff1, curr);	// rendered image with back culling
	vec4 rendWo = texture2D(txBuff3, curr);	// rendered image without back culling
	vec4 img = texture2D(txImage, curr);	// original image

	float tranIn = texture2D(txTran1, curr).x;		// transition inside
	float tranOut = texture2D(txTran2, curr).x;		// transition outside
	float line2d = texture2D(txEdge, curr).x;		// image lines
	float line3dWo = texture2D(txLine3D1, curr).x;	// 3d lines without back culling
	float line3dW = texture2D(txLine3D2, curr).x;	// 3d lines with back culling
	float mask = texture2D(txMask, curr).x;

	//// color composition 
	float intenClrAlp = transition_inside_curvature();
	
	vec4 clrComp = mix(img, rendW* selected_line3d(), intenClrAlp);	
//	return vec4(intenClrAlp);

	// composite with lines
	float lineAlp = tranOut; //smoothstep(0.0,0.65,tranOut);
	//return vec4(lineAlp);

	lineAlp = clamp( 1.0-lineAlp + line2d, 0.0, 1.0);
	float lineAlp02 = lineAlp;

	vec4 fin = clrComp * lineAlp02;

	return mix(fin, img, mask);
}



void main(void) 
{	
	gl_FragColor = blending();
}