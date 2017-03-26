uniform sampler2D ourTexture;
varying vec2 TexCoord;

void main (void)
{
    gl_FragColor = vec4 (texture(ourTexture, TexCoord).rgb, 1.0);
}
