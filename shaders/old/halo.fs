uniform sampler2D txLine3D;
uniform sampler2D txEdge;


uniform float ws;
uniform float hs;

void main(void) 
{	
	int rad = 4;

	float lineV = texture2D(txLine3D, gl_TexCoord[0].st).x;
	float lineR = texture2D(txEdge, gl_TexCoord[0].st).x;
	if(lineV==0.0)
	{

		for(int yi=-rad; yi<=rad; yi++)
		{
			if(lineV==1.0)	break;
			for(int xi=-rad; xi<=rad; xi++)
			{
				if(xi*xi+yi*yi>rad*rad)
					continue;
				float lr = texture2D(txEdge, gl_TexCoord[0].st + vec2(ws*float(xi), hs*float(yi))).x;
				if(lr<=0.6)
				{
					lineV = 1.0;
					break;
				}
			}
		}
	}
	//gl_FragColor = vec4(lineV*lineR);
	gl_FragColor = vec4(lineR);

}
