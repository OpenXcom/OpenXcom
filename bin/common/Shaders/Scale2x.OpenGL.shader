language: "GLSL"
vertex: |
    uniform vec2 rubyTextureSize;

    void main() {
      vec4 offsetx;
      vec4 offsety;

      gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

      offsetx.x = 1.0 / rubyTextureSize.x;
      offsetx.y = 0.0;
      offsetx.w = 0.0;
      offsetx.z = 0.0;
      offsety.y = 1.0 / rubyTextureSize.y;
      offsety.x = 0.0;
      offsety.w = 0.0;
      offsety.z = 0.0;

      gl_TexCoord[0] = gl_MultiTexCoord0;         //center
      gl_TexCoord[1] = gl_TexCoord[0] - offsetx;  //left
      gl_TexCoord[2] = gl_TexCoord[0] + offsetx;  //right
      gl_TexCoord[3] = gl_TexCoord[0] - offsety;  //top
      gl_TexCoord[4] = gl_TexCoord[0] + offsety;  //bottom
    }
fragment: |
    uniform sampler2D rubyTexture;
    uniform vec2 rubyTextureSize;

    void main() {
      vec4 colD, colF, colB, colH, col, tmp;
      vec2 sel;

      col  = texture2DProj(rubyTexture, gl_TexCoord[0]);  //central (can be E0-E3)
      colD = texture2DProj(rubyTexture, gl_TexCoord[1]);  //D (left)
      colF = texture2DProj(rubyTexture, gl_TexCoord[2]);  //F (right)
      colB = texture2DProj(rubyTexture, gl_TexCoord[3]);  //B (top)
      colH = texture2DProj(rubyTexture, gl_TexCoord[4]);  //H (bottom)

      sel = fract(gl_TexCoord[0].xy * rubyTextureSize.xy);       //where are we (E0-E3)?
                                                                 //E0 is default
      if(sel.y >= 0.5) { tmp = colB; colB = colH; colH = tmp; }  //E1 (or E3): swap B and H
      if(sel.x >= 0.5) { tmp = colF; colF = colD; colD = tmp; }  //E2 (or E3): swap D and F

      if(colB == colD && colB != colF && colD != colH) {  //do the Scale2x rule
        col = colD;
      }

      gl_FragColor = col;
    }
linear: false