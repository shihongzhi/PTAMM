varying vec3 normal;
varying vec3 lightDir;
varying vec3 eyeVec;

void diffuse_shading()
{
	vec4 finalColor = (gl_LightSource[0].ambient*gl_FrontMaterial.ambient);

	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);

	float lambertTerm = dot(N,L);

	if(lambertTerm>=0.0) 
	{
		finalColor += gl_LightSource[0].diffuse*gl_FrontMaterial.diffuse*lambertTerm;	
		
		//vec3 E = normalize(eyeVec);
		//vec3 R = reflect(-L, N);
		//float specular = pow(max(dot(R, E),0.0),32.0);//gl_FrontMaterial.shininess);
		//finalColor += gl_LightSource[0].specular*gl_FrontMaterial.specular*specular;
	}

	gl_FragColor = finalColor;
}

void main(void) 
{
	diffuse_shading();
}
