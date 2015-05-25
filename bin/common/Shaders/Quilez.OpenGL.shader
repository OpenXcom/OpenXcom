#<!--
#    Fragment shader based on "Improved texture interpolation" by Iñigo Quílez
#
#    Original description:
#        http://www.iquilezles.org/www/articles/texture/texture.htm
#-->
language: GLSL
linear: true
fragment: |
        uniform sampler2D rubyTexture;
        uniform vec2 rubyInputSize;
        uniform vec2 rubyOutputSize;
        uniform vec2 rubyTextureSize;

        vec4 getTexel(vec2 p)
        {
                p = p * rubyTextureSize + vec2(0.5);

                vec2 i = floor(p);
                vec2 f = p - i;
                f = f * f * f * (f * (f * 6.0 - vec2(15.0)) + vec2(10.0));
                p = i + f;

                p = (p - vec2(0.5)) / rubyTextureSize;
                return texture2D(rubyTexture, p);
        }

        void main(void)
        {
                gl_FragColor = getTexel(gl_TexCoord[0].xy);
        }
