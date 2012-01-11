uniform sampler2D txPhong;
uniform sampler2D txAlpha;
uniform sampler2D txImage;
uniform sampler2D txCanny;
uniform float alpha;

void main(void) 
{
  vec4 phong = texture2D(txPhong, gl_TexCoord[0].st);
  float cut = texture2D(txAlpha, gl_TexCoord[0].st).x;
  vec4 img = texture2D(txImage, gl_TexCoord[0].st);
  vec4 canny = texture2D(txCanny, gl_TexCoord[0].st);


  gl_FragColor = img;
}
