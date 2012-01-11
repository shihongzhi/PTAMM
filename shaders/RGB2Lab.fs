// work with a default vertex shader

uniform sampler2D txImage;

vec3 RGB2Lab(vec3 rgbClr) // [0,1] -> [0,1] for all channels
{
	mat3 matrix = mat3(0.433910, 	0.212649, 0.017756,
                      0.376220, 0.715169, 0.1094782,
                      0.189860, 0.07218, 	0.872915);

	vec3 tmp = matrix*rgbClr;	
	float L, A, B;
	float Y = pow(tmp.y, .3333333);
	
	if (tmp.y > .008856) 
		L = 116.  * Y - 16.;
	else                  
	 	L = 903.3 * tmp.y;
	
	L/=100.0;										// [0,1.0]
	A = (2.083333*(pow(tmp.x, .3333333)-Y));		// [-0.5,0.5]
	B = (.8333333*(Y-pow(tmp.z, .3333333)));		// [-0.5,0.5]
 	return vec3(L, A, B);
}

void main()
{
	// BGR input 
	vec3 bgr = texture2D(txImage, gl_TexCoord[0].st).rgb;
	vec3 lab = RGB2Lab(bgr);
	gl_FragColor = vec4(lab,0.0);
}
