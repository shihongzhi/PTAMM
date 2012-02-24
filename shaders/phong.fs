#extension GL_ARB_texture_rectangle : enable
//uniform sampler2DRect ShadowMap;
uniform sampler2DShadow ShadowMap;
// This define the value to move one pixel left or right
uniform float xPixelOffset ;
// This define the value to move one pixel up or down
uniform float yPixelOffset ;
uniform float shadowVariable;


varying vec3 normal;
varying vec3 lightDir;
varying vec3 eyeVec;
varying vec4 ShadowCoord;

float lookup( vec2 offSet)
{
	// Values are multiplied by ShadowCoord.w because shadow2DProj does a W division for us.
	return shadow2DProj(ShadowMap, ShadowCoord + vec4(offSet.x * xPixelOffset * ShadowCoord.w, offSet.y * yPixelOffset * ShadowCoord.w, 0.05, 0.0) ).w;
}

void main(void) {
	//phong shader
    vec4 finalColor = (gl_LightSource[0].ambient*gl_FrontMaterial.ambient);
  
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
  
    float lambertTerm = max(dot(N,L),0.0);
  
    if(lambertTerm>=0.0) {
        finalColor += gl_LightSource[0].diffuse*gl_FrontMaterial.diffuse*lambertTerm;
    
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow(max(dot(R, E),0.0),32.0);//gl_FrontMaterial.shininess);
		finalColor += gl_LightSource[0].specular*gl_FrontMaterial.specular*specular;
    }
	//end of phong shader
  
	float shadow ;
	
	// Avoid counter shadow  8*8=64Kernel
	if (ShadowCoord.w > 1.0)
	{
		float x,y;
		for (y = -3.5 ; y <=3.5 ; y+=1.0)
			for (x = -3.5 ; x <=3.5 ; x+=1.0)
				shadow += lookup(vec2(x,y));
		
		shadow /= 64.0 ;
	}
  	gl_FragColor = (shadow+shadowVariable) * finalColor;
  //gl_FragData[0] = shadow*finalColor;
  //gl_FragColor = shadow*gl_Color;
  //gl_FragData[0] = vec4(distanceFromLight);  //深度图没发看，因为没有归一化
  //gl_FragData[1] = vec4(0.0,0.0,0.0,1.0);
}
