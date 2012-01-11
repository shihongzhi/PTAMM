#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect txBuff1;
//uniform sampler2DRect txBuff3;
//uniform sampler2DRect txLine3D1;
//uniform sampler2DRect txLine3D2;
uniform sampler2DRect txImage;
//uniform sampler2DRect txTRAN1;
//uniform sampler2DRect txTRAN2;
uniform sampler2DRect txBackEdge;

void main(void)
{
	vec2 curr = gl_TexCoord[0].st;
	vec4 img = texture2DRect(txImage, curr);
	vec4 object = texture2DRect(txBuff1, curr);
	/*vec4 line3d1 = texture2DRect(txLine3D1, curr);
	vec4 line3d2 = texture2DRect(txLine3D2, curr);
	vec4 tran1 = texture2DRect(txTRAN1, curr);
	vec4 tran2 = texture2DRect(txTRAN2, curr);*/
	vec4 backEdge = texture2DRect(txBackEdge, curr);

	if(object.r == 0.0)
	{
		gl_FragColor = img;
	}
	else
	{
		if(object.a==0.0)
		{
			if(vec3(backEdge.rgb) == vec3(1.0))
				gl_FragColor = vec4(161.0/256.0, 155.0/256.0, 139.0/256.0, 1.0);  //这里的颜色设置成为旁边桌面的颜色
			else
				gl_FragColor = backEdge;
		}
		else
		{
			gl_FragColor = object;
		}
		//gl_FragColor = object;
	}
	//gl_FragColor = object;
}
