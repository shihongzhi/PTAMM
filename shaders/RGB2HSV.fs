// work with a default vertex shader

uniform sampler2D txRGB;

vec3 RGB2HSV(vec3 rgb)
{
    float R = rgb.r * 255.;
    float G = rgb.g * 255.;
    float B = rgb.b * 255.;
    
	float maxV = max(max(R,G),B);
	float minV = min(min(R,G),B);
    
	float H=0.0;
    if (int(maxV) == int(R))        H = (G-B)/(maxV-minV);
    else if (int(maxV) == int(G))   H = 2.0 + (B-R)/(maxV-minV);
    else							H = 4.0 + (R-G)/(maxV-minV);

	H = H * 60.0;
    if (H < 0.) H+=360.0;
	if (H>360.0) H-=360.0;

	float S = (maxV-minV)/maxV;
	float V = maxV;

    return vec3(H/520.0, S, V/255.);
}

void main()
{
	// BGR input 
	vec3 bgr = texture2D(txRGB, gl_TexCoord[0].st).rgb;
	vec3 hsv = RGB2HSV(bgr);
	gl_FragColor = vec4(hsv,0.0);
}
