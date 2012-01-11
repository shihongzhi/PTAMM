
uniform float ws;
uniform float hs;
uniform int iCurt;
uniform int curtSize;

void main()
{
	if(iCurt==0)	gl_FragColor = vec4(1.0);
	else
	{
		vec2 curr = gl_TexCoord[0].st;

		vec2 doorOri = vec2(1.17, 1.2)*float(curtSize);
		vec2 pos = vec2((curr.x-0.5)/ws, curr.y/hs);
		// screen door
		vec2 door = doorOri*(1.0-curr.y*1.2);
		float doorAlp = (fract((pos.x)/door.y)<=door.x/door.y && fract(pos.y/door.y)<=(1.0-curr.y*0.5)*door.x/door.y) ? 1.0:0.0;

		// window louver 
		//float doorAlp = fract(pos.y/door.y)<=(1.0-curr.y*0.75)*door.x/door.y ? 1.0:0.0;

		// grid dot
		//door *= 1.5;
		//float doorAlp = (fract(pos.x/door.y)<=door.x/door.y || fract(pos.y/door.y)<=(1.0-curr.y*0.5)*door.x/door.y) ? 1.0:0.0;

		// circle
		//vec2 pnt = vec2(fract((pos.x)/door.y)-0.5, (fract((pos.y)/door.y)-0.5)/(1.0-curr.y*0.75));
		//float doorAlp = length(pnt)<=door.x*0.25/door.y ? 0.0:1.0;

		gl_FragColor = vec4(doorAlp);
		//gl_FragColor = texture2D(curr, gl_TexCoord[0].st);
	}
}