#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txImage;


void main(void) 
{
	float curr = texture2DRect(txImage, gl_TexCoord[0].st).r;
	float l = curr-texture2DRect(txImage, gl_TexCoord[0].st+vec2(-1.0,0.0)).r;
	float r = curr-texture2DRect(txImage, gl_TexCoord[0].st+vec2(1.0,0.0)).r;
	float t = curr-texture2DRect(txImage, gl_TexCoord[0].st+vec2(0.0,-1.0)).r;
	float b = curr-texture2DRect(txImage, gl_TexCoord[0].st+vec2(0.0,1.0)).r;

	gl_FragColor = vec4( (l+r+t+b)/4.0 >= 0.03? 0.0:1.0 );  //这里的0.015参数需要根据实际情况修改
}
