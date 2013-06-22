#<!--
#    Blank shader to allow Vsync without changing anything.
#-->
language: GLSL
linear: false
fragment: |
        uniform sampler2D rubyTexture;

        void main(void)
        {
                gl_FragColor = texture2D(rubyTexture, gl_TexCoord[0].xy);
        }
