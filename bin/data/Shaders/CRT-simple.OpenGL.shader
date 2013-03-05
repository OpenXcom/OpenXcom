#<!--
#    CRT-simple shader
#
#    Copyright (C) 2011 DOLLS. Based on cgwg's CRT shader.
#
#    This program is free software; you can redistribute it and/or modify it
#    under the terms of the GNU General Public License as published by the Free
#    Software Foundation; either version 2 of the License, or (at your option)
#    any later version.
#    -->
language: GLSL
vertex: |
        uniform vec2 rubyInputSize;
        uniform vec2 rubyOutputSize;
        uniform vec2 rubyTextureSize;

        // Define some calculations that will be used in fragment shader.
        varying vec2 texCoord;
        varying vec2 one;
        varying float mod_factor;

        void main()
        {
                // Do the standard vertex processing.
                gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

                // Precalculate a bunch of useful values we'll need in the fragment
                // shader.

                // Texture coords.
                texCoord = gl_MultiTexCoord0.xy;

                // The size of one texel, in texture-coordinates.
                one = 1.0 / rubyTextureSize;

                // Resulting X pixel-coordinate of the pixel we're drawing.
                mod_factor = texCoord.x * rubyTextureSize.x * rubyOutputSize.x / rubyInputSize.x;
        }
linear: true
fragment: |
        uniform sampler2D rubyTexture;
        uniform vec2 rubyInputSize;
        uniform vec2 rubyOutputSize;
        uniform vec2 rubyTextureSize;

        varying vec2 texCoord;
        varying vec2 one;
        varying float mod_factor;

        // Enable screen curvature.
        #define CURVATURE

        // Controls the intensity of the barrel distortion used to emulate the
        // curvature of a CRT. 0.0 is perfectly flat, 1.0 is annoyingly
        // distorted, higher values are increasingly ridiculous.
        #define distortion 0.1

        // Simulate a CRT gamma of 2.4.
        #define inputGamma  2.4

        // Compensate for the standard sRGB gamma of 2.2.
        #define outputGamma 2.2

        // Macros.
        #define TEX2D(c) pow(texture2D(rubyTexture, (c)), vec4(inputGamma))
        #define PI 3.141592653589

        // Apply radial distortion to the given coordinate.
        vec2 radialDistortion(vec2 coord)
        {
                coord *= rubyTextureSize / rubyInputSize;
                vec2 cc = coord - 0.5;
                float dist = dot(cc, cc) * distortion;
                return (coord + cc * (1.0 + dist) * dist) * rubyInputSize / rubyTextureSize;
        }

        // Calculate the influence of a scanline on the current pixel.
        //
        // 'distance' is the distance in texture coordinates from the current
        // pixel to the scanline in question.
        // 'color' is the colour of the scanline at the horizontal location of
        // the current pixel.
        vec4 scanlineWeights(float distance, vec4 color)
        {
                // The "width" of the scanline beam is set as 2*(1 + x^4) for
                // each RGB channel.
                vec4 wid = 2.0 + 2.0 * pow(color, vec4(4.0));

                // The "weights" lines basically specify the formula that gives
                // you the profile of the beam, i.e. the intensity as
                // a function of distance from the vertical center of the
                // scanline. In this case, it is gaussian if width=2, and
                // becomes nongaussian for larger widths. Ideally this should
                // be normalized so that the integral across the beam is
                // independent of its width. That is, for a narrower beam
                // "weights" should have a higher peak at the center of the
                // scanline than for a wider beam.
                vec4 weights = vec4(distance / 0.3);
                return 1.4 * exp(-pow(weights * inversesqrt(0.5 * wid), wid)) / (0.6 + 0.2 * wid);
        }

        void main()
        {
                // Here's a helpful diagram to keep in mind while trying to
                // understand the code:
                //
                //  |      |      |      |      |
                // -------------------------------
                //  |      |      |      |      |
                //  |  01  |  11  |  21  |  31  | <-- current scanline
                //  |      | @    |      |      |
                // -------------------------------
                //  |      |      |      |      |
                //  |  02  |  12  |  22  |  32  | <-- next scanline
                //  |      |      |      |      |
                // -------------------------------
                //  |      |      |      |      |
                //
                // Each character-cell represents a pixel on the output
                // surface, "@" represents the current pixel (always somewhere
                // in the bottom half of the current scan-line, or the top-half
                // of the next scanline). The grid of lines represents the
                // edges of the texels of the underlying texture.

                // Texture coordinates of the texel containing the active pixel.
        #ifdef CURVATURE
                vec2 xy = radialDistortion(texCoord);
        #else
                vec2 xy = texCoord;
        #endif

                // Of all the pixels that are mapped onto the texel we are
                // currently rendering, which pixel are we currently rendering?
                vec2 ratio_scale = xy * rubyTextureSize - vec2(0.5);
                vec2 uv_ratio = fract(ratio_scale);

                // Snap to the center of the underlying texel.
                xy.y = (floor(ratio_scale.y) + 0.5) / rubyTextureSize.y;

                // Calculate the effective colour of the current and next
                // scanlines at the horizontal location of the current pixel.
                vec4 col  = TEX2D(xy);
                vec4 col2 = TEX2D(xy + vec2(0.0, one.y));

                // Calculate the influence of the current and next scanlines on
                // the current pixel.
                vec4 weights  = scanlineWeights(uv_ratio.y, col);
                vec4 weights2 = scanlineWeights(1.0 - uv_ratio.y, col2);
                vec3 mul_res  = (col * weights + col2 * weights2).rgb;

                // dot-mask emulation:
                // Output pixels are alternately tinted green and magenta.
                vec3 dotMaskWeights = mix(
                        vec3(1.0, 0.7, 1.0),
                        vec3(0.7, 1.0, 0.7),
                        floor(mod(mod_factor, 2.0))
                    );

                mul_res *= dotMaskWeights;

                gl_FragColor = vec4(pow(mul_res, vec3(1.0 / outputGamma)), 1.0);
        }
