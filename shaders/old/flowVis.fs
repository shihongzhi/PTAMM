
uniform sampler2D txFlowVis;

vec3 RGB2HSV(vec3 rgb)
{
/*
V=max(R,G,B)
S=(V-min(R,G,B))*255/V   if V!=0, 0 otherwise

       (G - B)*60/S,  if V=R
H= 180+(B - R)*60/S,  if V=G
   240+(R - G)*60/S,  if V=B

if H<0 then H=H+360*/   

    float R = rgb.r * 255.;
    float G = rgb.g * 255.;
    float B = rgb.b * 255.;
    
    float H;
    float V = max(max(R, G), B);
    float S;

    if (V == 0.) S = 0.;
    else S = (V-min(min(R, G), B))* 255. / V;
    
    if (int(V) == int(R))        H = (G-B)*60./S;
    else if (int(V) == int(G))   H = 180. + (B-R)*60./S;
    else                         H = 240. + (R-G)*60./S;


    if (H < 0.) H+=360.;

//The hue values calcualted using the above formulae vary from 0?to 360?
//so they are divided by 2 to fit into 8-bit destination format.
    return vec3(H/512., S/255., V/255.);
}

vec3 HSV2RGB(vec3 hsv)
{
	int hi = int(floor( hsv.x*512./60. ));
	if(hi<0) 
		hi += 6;
	float f = fract(hsv.x*512./60.);
	
	float p = hsv.z * (1.0 - hsv.y);
	float q = hsv.z * (1.0 - f*hsv.y);
	float t = hsv.z * (1.0 - (1.0-f)*hsv.y);
	
	vec3 rgb;
	if(hi==0)
		rgb = vec3(hsv.z, t, p);
	else if(hi==1)
		rgb = vec3(q, hsv.z, p);
	else if(hi==2)
		rgb = vec3(p, hsv.z, t);
	else if(hi==3)
		rgb = vec3(p, q, hsv.z);
	else if(hi==4)
		rgb = vec3(t, p, hsv.z);
	else if(hi==5)
		rgb = vec3(hsv.z, p, q);
	else
		rgb = vec3(1.0, 1.0, 1.0);
		
	return rgb;
}

vec2 remap(vec2 orien)
{
	float hue = degrees( atan(orien.y,orien.x) ) + 180.;
	float len = length(orien);
	
	float B = 8.;
	float alpha = 100000.0;
	float K = atanh((exp2(B)-2.0)/(exp2(B)-1.0));
	float saturation;
	if(len<=alpha) 
		saturation = tanh( len*K/alpha );
	else
		saturation = 1.0;
	
	return vec2(hue/512.,saturation);
}

void FlowVis()
{
	vec2 tangent = texture2D(txSmoothTang_LOE, gl_TexCoord[0].st).xy;

	//vec3 hsv = RGB2HSV(rgb);
	
	//tangent = vec2(-100.0, -100.0);
	
	vec3 rgb;
	if(length(tangent)>0.0)	
	{
		vec2 hs = remap(tangent);
		rgb = HSV2RGB(vec3(hs.x, hs.y, 1.0));
	}
	else
		rgb = HSV2RGB(vec3(1.0, 1.0, 1.0));
		
	gl_FragColor = vec4(rgb,1.0);
}

void main()
{
	FlowVis();
}