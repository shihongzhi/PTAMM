varying vec3 normal;
varying vec3 lightDir;
varying vec3 eyeVec;

void main() {	
  normal = gl_NormalMatrix * gl_Normal;
  //normal = gl_Normal;
  
  vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
  
  lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
  eyeVec   = -vVertex;
  
  gl_Position = ftransform();		
}
