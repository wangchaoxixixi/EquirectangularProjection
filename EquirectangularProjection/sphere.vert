varying vec2 TexCoord;

void main(void)
{
    TexCoord.x = 1.0 - gl_MultiTexCoord0.st.x;
    TexCoord.y = gl_MultiTexCoord0.st.y;
    gl_Position  = ftransform();
}
