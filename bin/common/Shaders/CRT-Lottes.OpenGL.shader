#<!--
# PUBLIC DOMAIN CRT STYLED SCAN-LINE SHADER
#
#   by Timothy Lottes
#
# This is more along the style of a really good CGA arcade monitor.
# With RGB inputs instead of NTSC.
# The shadow mask example has the mask rotated 90 degrees for less chromatic aberration.
#
# Left it unoptimized to show the theory behind the algorithm.
#
# It is an example what I personally would want as a display option for pixel art games.
# Please take and use, change, or whatever.
#
# Version without screen curvature emulation ('warp')
# Adapted for OpenXcom by somnolik
#-->
language: GLSL
vertex: |
        #version 110
        uniform vec2 rubyOutputSize;

        varying vec2 v_texCoord;
        varying vec2 screenPixel;

        void main(void)
        {
                gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                v_texCoord = gl_MultiTexCoord0.xy;
                screenPixel = gl_Position.xy * rubyOutputSize;
        }
linear: false
fragment: |
        #version 110
        uniform sampler2D rubyTexture;
        uniform vec2 rubyTextureSize;

        varying vec2 v_texCoord;
        varying vec2 screenPixel;


        /*
        // Shader parameters for RetroArch
        // Syntax:
        // #pragma parameter <shortName> <longName> <defaultValue> <min> <max> <step>

        #pragma parameter hardScan "hardScan" -8.0 -20.0 0.0 1.0
        #pragma parameter hardPix "hardPix" -3.0 -20.0 0.0 1.0

        #pragma parameter maskDark "maskDark" 0.5 0.0 2.0 0.1
        #pragma parameter maskLight "maskLight" 1.5 0.0 2.0 0.1
        #pragma parameter shadowMask "shadowMask" 3.0 0.0 4.0 1.0
        #pragma parameter brightBoost "brightness boost" 1.0 0.0 2.0 0.05

        #pragma parameter hardBloomPix "bloom-x soft" -1.5 -2.0 -0.5 0.1
        #pragma parameter hardBloomScan "bloom-y soft" -2.0 -4.0 -1.0 0.1
        #pragma parameter bloomAmount "bloom ammount" 0.15 0.0 1.0 0.05
        #pragma parameter shape "filter kernel shape" 2.0 0.0 10.0 0.05
        */

        #define hardScan -8.0        // decrease for better distinction of scanlines
        #define hardPix -5.0         // decrease for better distinction of individual pixels

        #define maskDark 0.8         // > 1 amplifies, < 1 attenuates other color channels: green and blue for a red pixel, etc.
        #define maskLight 1.5        // > 1 amplifies, < 1 attenuates own color channel: red for a red pixel, etc.
        #define shadowMask 3.0       // 0 - disable; 1 - TV-style; 2 - Aperture grille; 3 - stretched VGA; 4 - VGA
        #define brightBoost 1.0      // > 1 amplifies, < 1 attenuates general brightness; using maskDark and maskLight often looks nicer

        #define hardBloomPix -1.5    // increase for a larger horizontal bloom effect
        #define hardBloomScan -2.0   // increase for a larger vertical bloom effect
        #define bloomAmount 0.15     // increase for a stronger bloom effect
        #define shape 2.0


        float ToLinear1(float c)
        {
                return (c <= 0.04045) ? c / 12.92 : pow((c + 0.055) / 1.055, 2.4);
        }

        vec3 ToLinear(vec3 c)
        {
                return vec3(ToLinear1(c.r), ToLinear1(c.g), ToLinear1(c.b));
        }

        // Linear to sRGB.
        // Assuming using sRGB typed textures this should not be needed.
        float ToSrgb1(float c)
        {
                return (c < 0.0031308 ? c * 12.92 : 1.055 * pow(c, 0.41666) - 0.055);
        }

        vec3 ToSrgb(vec3 c)
        {
                return vec3(ToSrgb1(c.r), ToSrgb1(c.g), ToSrgb1(c.b));
        }


        // Nearest emulated sample given floating point position and texel offset.
        // Also zero's off screen.
        vec3 Fetch(vec2 pos, vec2 off){
                pos = (floor(pos * rubyTextureSize + off) + vec2(0.5, 0.5)) / rubyTextureSize;
                return ToLinear(brightBoost * texture2D(rubyTexture, pos.xy).rgb);
        }

        // Distance in emulated pixels to nearest texel.
        vec2 Dist(vec2 pos)
        {
                pos = pos * rubyTextureSize;
                
                return -((pos - floor(pos)) - vec2(0.5));
        }

        // 1D Gaussian.
        float Gaus(float pos, float scale)
        {
                return exp2(scale * pow(abs(pos), shape));
        }

        // 3-tap Gaussian filter along horz line.
        vec3 Horz3(vec2 pos, float off)
        {
                vec3 b    = Fetch(pos, vec2(-1.0, off));
                vec3 c    = Fetch(pos, vec2( 0.0, off));
                vec3 d    = Fetch(pos, vec2( 1.0, off));
                float dst = Dist(pos).x;

                // Convert distance to weight.
                float scale = hardPix;
                float wb = Gaus(dst - 1.0, scale);
                float wc = Gaus(dst + 0.0, scale);
                float wd = Gaus(dst + 1.0, scale);

                // Return filtered sample.
                return (b*wb + c*wc + d*wd) / (wb + wc + wd);
        }

        // 5-tap Gaussian filter along horz line.
        vec3 Horz5(vec2 pos,float off){
                vec3 a = Fetch(pos, vec2(-2.0, off));
                vec3 b = Fetch(pos, vec2(-1.0, off));
                vec3 c = Fetch(pos, vec2( 0.0, off));
                vec3 d = Fetch(pos, vec2( 1.0, off));
                vec3 e = Fetch(pos, vec2( 2.0, off));
                
                float dst = Dist(pos).x;
                // Convert distance to weight.
                float scale = hardPix;
                float wa = Gaus(dst - 2.0, scale);
                float wb = Gaus(dst - 1.0, scale);
                float wc = Gaus(dst + 0.0, scale);
                float wd = Gaus(dst + 1.0, scale);
                float we = Gaus(dst + 2.0, scale);
                
                // Return filtered sample.
                return (a*wa + b*wb + c*wc + d*wd + e*we) / (wa + wb + wc + wd + we);
        }

        // 7-tap Gaussian filter along horz line.
        vec3 Horz7(vec2 pos,float off)
        {
                vec3 a = Fetch(pos, vec2(-3.0, off));
                vec3 b = Fetch(pos, vec2(-2.0, off));
                vec3 c = Fetch(pos, vec2(-1.0, off));
                vec3 d = Fetch(pos, vec2( 0.0, off));
                vec3 e = Fetch(pos, vec2( 1.0, off));
                vec3 f = Fetch(pos, vec2( 2.0, off));
                vec3 g = Fetch(pos, vec2( 3.0, off));

                float dst = Dist(pos).x;
                // Convert distance to weight.
                float scale = hardBloomPix;
                float wa = Gaus(dst - 3.0, scale);
                float wb = Gaus(dst - 2.0, scale);
                float wc = Gaus(dst - 1.0, scale);
                float wd = Gaus(dst + 0.0, scale);
                float we = Gaus(dst + 1.0, scale);
                float wf = Gaus(dst + 2.0, scale);
                float wg = Gaus(dst + 3.0, scale);

                // Return filtered sample.
                return (a*wa + b*wb + c*wc + d*wd + e*we + f*wf + g*wg) / (wa + wb + wc + wd + we + wf + wg);
        }

        // Return scanline weight.
        float Scan(vec2 pos, float off)
        {
                float dst = Dist(pos).y;

                return Gaus(dst + off, hardScan);
        }

        // Return scanline weight for bloom.
        float BloomScan(vec2 pos, float off)
        {
                float dst = Dist(pos).y;
                
                return Gaus(dst + off, hardBloomScan);
        }

        vec3 Tri(vec2 pos)
        {
                vec3 sample = Fetch(pos, vec2(0.0, 0.0));

                vec3 a = Horz3(pos,-1.0);
                vec3 b = Horz5(pos, 0.0);
                vec3 c = Horz3(pos, 1.0);
                
                float wa = Scan(pos,-1.0); 
                float wb = Scan(pos, 0.0);
                float wc = Scan(pos, 1.0);
                
                return a*wa + b*wb + c*wc;
                        
                // return sample;
        }

        // Small bloom.
        vec3 Bloom(vec2 pos)
        {
                vec3 a = Horz5(pos,-2.0);
                vec3 b = Horz7(pos,-1.0);
                vec3 c = Horz7(pos, 0.0);
                vec3 d = Horz7(pos, 1.0);
                vec3 e = Horz5(pos, 2.0);

                float wa = BloomScan(pos,-2.0);
                float wb = BloomScan(pos,-1.0); 
                float wc = BloomScan(pos, 0.0);
                float wd = BloomScan(pos, 1.0);
                float we = BloomScan(pos, 2.0);

                return a*wa + b*wb + c*wc + d*wd + e*we;
        }

        // Shadow mask.
        vec3 Mask(vec2 pos)
        {
                vec3 mask = vec3(maskDark, maskDark, maskDark);
                
                // Very compressed TV style shadow mask.
                if (shadowMask == 1.0) 
                {
                        float line = maskLight;
                        float odd = 0.0;
                        
                        if (fract(pos.x*0.166666666) < 0.5) odd = 1.0;
                        if (fract((pos.y + odd) * 0.5) < 0.5) line = maskDark;  
                        
                        pos.x = fract(pos.x*0.333333333);

                        if      (pos.x < 0.333) mask.r = maskLight;
                        else if (pos.x < 0.666) mask.g = maskLight;
                        else                    mask.b = maskLight;
                        mask *= line;  
                } 

                // Aperture-grille.
                else if (shadowMask == 2.0) 
                {
                        pos.x = fract(pos.x*0.333333333);

                        if      (pos.x < 0.333) mask.r = maskLight;
                        else if (pos.x < 0.666) mask.g = maskLight;
                        else                    mask.b = maskLight;
                } 

                // Stretched VGA style shadow mask (same as prior shaders).
                else if (shadowMask == 3.0) 
                {
                        pos.x += pos.y * 3.0;
                        pos.x  = fract(pos.x * 0.166666666);

                        if      (pos.x < 0.333) mask.r = maskLight;
                        else if (pos.x < 0.666) mask.g = maskLight;
                        else                    mask.b = maskLight;
                }

                // VGA style shadow mask.
                else if (shadowMask == 4.0) 
                {
                        pos.xy  = floor(pos.xy * vec2(1.0, 0.5));
                        pos.x  += pos.y * 3.0;
                        pos.x   = fract(pos.x * 0.166666666);

                        if      (pos.x < 0.333) mask.r = maskLight;
                        else if (pos.x < 0.666) mask.g = maskLight;
                        else                    mask.b = maskLight;
                }

                return mask;
        }

        void main() {
                vec2 pos = v_texCoord.xy;
                vec3 outColor = Tri(pos);

                // Add Bloom
                outColor.rgb += Bloom(pos) * bloomAmount;

                if (shadowMask > 0.0)
                {
                        outColor.rgb *= Mask(screenPixel);
                }

                gl_FragColor = vec4(ToSrgb(outColor.rgb), 1.0);
        }
