uniform float zminObj;
uniform float zmaxObj;

varying vec3 normal;
varying vec3 lightDir;
varying vec3 eyeVec;
varying float objDep;

void main() {	 
  normal = gl_NormalMatrix * gl_Normal;
  
  vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

  objDep = clamp( (gl_Vertex.y-zminObj)/(zmaxObj-zminObj), 0.0, 1.0);

  lightDir = gl_LightSource[0].position.xyz;
  //lightDir = vec3((gl_ModelViewMatrix *vec4(0,10,0,0)).xyz - vVertex);
  //lightDir = (gl_ModelViewMatrix *vec4(0,1,0,0)).xyz;
  eyeVec   = -vVertex;

  gl_TexCoord[0] = gl_MultiTexCoord0;
  
  gl_Position = ftransform();		
}
