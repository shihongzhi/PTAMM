uniform sampler2D txXToon;

varying vec3 normal;
varying vec3 lightDir;
varying vec3 eyeVec;

varying float objDep;
//varying float eyeDep;

void main(void) 
{
  vec4 finalColor = (gl_LightSource[0].ambient*gl_FrontMaterial.ambient);
  
  vec3 N = normalize(normal);
  vec3 L = normalize(lightDir);
  
  float cosVal = (1+dot(N,L))*0.5;
  float lambertTerm = max(cosVal,0.0);
  
  if(lambertTerm>=0.0) {
    finalColor += gl_LightSource[0].diffuse*gl_FrontMaterial.diffuse*lambertTerm;
    
    vec3 E = normalize(eyeVec);
    vec3 R = reflect(-L, N);
    float specular = pow(max(dot(R, E),0.0),32.0);//gl_FrontMaterial.shininess);
    finalColor += gl_LightSource[0].specular*gl_FrontMaterial.specular*specular;
  }




  // x-toon shading using dot(N,L) and eyeDep
  float inter = clamp(dot(N,L),0.0,1.0);
  //if the face is back then converse the N
  if(inter == 0.0)
	  inter = clamp(dot(-N,L),0.0,1.0);
  vec3 newColor = texture2D(txXToon,vec2(inter,objDep)).rgb;

  //gl_FragData[0] = vec4(newColor*objDep, 1.0);
  gl_FragData[0] = vec4(newColor, 1.0);
  gl_FragData[1] = vec4(objDep,0.0,0.0,1.0);	

  //phong shading
  gl_FragData[0] = finalColor;
  gl_FragData[1] = vec4(objDep,0.0,0.0,1.0);
}
