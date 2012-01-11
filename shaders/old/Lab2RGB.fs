// work with a default vertex shader

uniform sampler2D txOrg;
uniform sampler2D txLOESmooth;
//uniform sampler2D txBlur;
uniform sampler2D txEdge;

uniform float ws;
uniform float hs;
uniform int q;
uniform int display;
uniform float phi_q;

vec3 Lab2RGB(vec3 lab)
{
	lab.x *= 100.0;
	lab.y = (lab.y)/2.083333;
	lab.z = (lab.z)/0.8333333;
	
	vec3 color;
	float Y =0.0;
	if(lab.x>7.999629)
	{
		Y = float(lab.x+16.)/116.;
		color.y = pow(Y,float(3.0));
	}
	else
	{
		color.y = lab.x/903.3;
		Y = pow(float(color.y), float(0.3333333));
	}
	color.x = pow(Y+lab.y,float(3.0));
	color.z = pow(Y-lab.z,float(3.0));

	mat3 rev = mat3(	3.0802214,	-0.92121303,	0.052880455,
						-1.5372765,		1.8759564,		-0.20400620,
						-0.54283303,	0.045240626,	1.1509547);
	
	return rev*color;
}

//float GradMag()
//{
//	float top = texture2D(txBlur, gl_TexCoord[0].st+(0.0,-hs)).r;
//	float btm = texture2D(txBlur, gl_TexCoord[0].st+(0.0, hs)).r;
//	float lft = texture2D(txBlur, gl_TexCoord[0].st+(-ws,0.0)).r;
//	float rgt = texture2D(txBlur, gl_TexCoord[0].st+( ws,0.0)).r;
//	return (abs(top-btm)+abs(lft-rgt)+0.001);
//}
float quan(float l)
{
	float idx = floor(l*float(q)+0.5);
	float qnear = clamp(idx/float(q),0.0,1.0);
	float delta = tanh(phi_q*(l-qnear)*3.0)*0.5 / float(q);
	return qnear+delta;
}
vec3 Quantization(in vec4 lab)
{
	float lumi = quan(lab.x*2.55)/2.55;
	return Lab2RGB(vec3(lumi,lab.yz));
}

void main()
{
	gl_FragColor = vec4(Lab2RGB(texture2D(txBlur, gl_TexCoord[0].st).rgb),0.0);
	//gl_FragColor = vec4(Quantization(texture2D(txBlur, gl_TexCoord[0].st)),0.0);
}
