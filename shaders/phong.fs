#extension GL_ARB_texture_rectangle : enable
uniform sampler2D ShadowMap;
//uniform float shadowVariable;


varying vec3 normal;
varying vec3 lightDir;
varying vec3 eyeVec;
varying vec4 ShadowCoord;


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
	if(gl_Color.a==0.0 && gl_Color.r > 0.82)  //有边缘提取的
	{
		finalColor = vec4(161.0/256.0, 155.0/256.0, 139.0/256.0, 0.0);
	}
	else if(gl_Color.a==0.0 && gl_Color.r > 0.67)  //无边缘提取的
	{
		finalColor = vec4(161.0/256.0, 155.0/256.0, 139.0/256.0, 1.0);
	}
	else //其余为物体和阴影，但是边缘提取的阴影不知道为什么没有了
	{
		finalColor = vec4(finalColor.rgb, 1.0);
	}

	vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
		
	// Used to lower moiré pattern and self-shadowing  这个阈值变小之后可以把书本旁边的齿轮变小，但是没法消掉，不知道怎么消
	shadowCoordinateWdivide.z += 0.0000001;
	
	float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.st).z;
	
 	float shadow = 1.0;
 	if (ShadowCoord.w > 0.0)
 		shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;
  	
	gl_FragColor =	 shadow * finalColor;
}
