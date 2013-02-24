#<!--
#     Dot 'n bloom shader
#     Author: Themaister
#     License: Public domain
#-->
language: GLSL
vertex: |
      varying vec2 c00;
      varying vec2 c10;
      varying vec2 c20;
      varying vec2 c01;
      varying vec2 c11;
      varying vec2 c21;
      varying vec2 c02;
      varying vec2 c12;
      varying vec2 c22;
      varying vec2 pixel_no;
      uniform vec2 rubyTextureSize;
      uniform vec2 rubyOutputSize;
      uniform vec2 rubyInputSize;

      void main()
      {
         gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
         float dx = 1.0 / rubyTextureSize.x;
         float dy = 1.0 / rubyTextureSize.y;

         vec2 tex = gl_MultiTexCoord0.xy;
         c00 = tex + vec2(-dx, -dy);
         c10 = tex + vec2(  0, -dy);
         c20 = tex + vec2( dx, -dy);
         c01 = tex + vec2(-dx,   0);
         c11 = tex + vec2(  0,   0);
         c21 = tex + vec2( dx,   0);
         c02 = tex + vec2(-dx,  dy);
         c12 = tex + vec2(  0,  dy);
         c22 = tex + vec2( dx,  dy);
         pixel_no = tex * rubyTextureSize;
      }
linear: false
fragment: |
      uniform vec2 rubyTextureSize;
      uniform sampler2D rubyTexture;
      varying vec2 c00;
      varying vec2 c10;
      varying vec2 c20;
      varying vec2 c01;
      varying vec2 c11;
      varying vec2 c21;
      varying vec2 c02;
      varying vec2 c12;
      varying vec2 c22;
      varying vec2 pixel_no;

      const float gamma = 2.4;
      const float shine = 0.50;
      const float blend = 0.25;

      float dist(vec2 coord, vec2 source)
      {
         vec2 delta = coord - source;
         return sqrt(dot(delta, delta));
      }

      float color_bloom(vec3 color)
      {
         const vec3 gray_coeff = vec3(0.30, 0.59, 0.11);
         float bright = dot(color, gray_coeff);
         return mix(1.0 + shine, 1.0 - shine, bright);
      }

      vec3 lookup(float offset_x, float offset_y, vec2 coord)
      {
         vec2 offset = vec2(offset_x, offset_y);
         vec3 color = texture2D(rubyTexture, coord).rgb;
         float delta = dist(fract(pixel_no), offset + vec2(0.5));
         return color * exp(-gamma * delta * color_bloom(color));
      }

      void main()
      {
         vec3 mid_color = lookup(0.0, 0.0, c11);
         vec3 color = vec3(0.0);
         color += lookup(-1.0, -1.0, c00);
         color += lookup( 0.0, -1.0, c10);
         color += lookup( 1.0, -1.0, c20);
         color += lookup(-1.0,  0.0, c01);
         color += mid_color;
         color += lookup( 1.0,  0.0, c21);
         color += lookup(-1.0,  1.0, c02);
         color += lookup( 0.0,  1.0, c12);
         color += lookup( 1.0,  1.0, c22);
         vec3 out_color = mix(1.2 * mid_color, color, blend);

         gl_FragColor = vec4(out_color, 1.0);
      }
