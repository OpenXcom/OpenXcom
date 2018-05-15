# <?xml version="1.0" encoding="UTF-8"?>
# <!--
    # Hyllian's 5xBR v3.7b (semi-rounded) Shader

   # Copyright (C) 2011, 2012 Hyllian/Jararaca - sergiogdb@gmail.com
   # Copyright (C) 2012 crazy46guy (GLSL conversion)

   # This program is free software; you can redistribute it and/or
   # modify it under the terms of the GNU General Public License
   # as published by the Free Software Foundation; either version 2
   # of the License, or (at your option) any later version.

   # This program is distributed in the hope that it will be useful,
   # but WITHOUT ANY WARRANTY; without even the implied warranty of
   # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   # GNU General Public License for more details.

   # You should have received a copy of the GNU General Public License
   # along with this program; if not, write to the Free Software
   # Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   # (The original Cg version of the xBR family of shaders is available on
   # Github: https://github.com/twinaphex/common-shaders/tree/master/xBR )
# -->
language: "GLSL"
vertex: |
    uniform vec2 rubyTextureSize;

    void main() {
      float dx = 1.0 / rubyTextureSize.x;
      float dy = 1.0 / rubyTextureSize.y;

      //     A1 B1 C1
      //  A0  A  B  C C4
      //  D0  D  E  F F4
      //  G0  G  H  I I4
      //     G5 H5 I5

      gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
      gl_TexCoord[0] = gl_MultiTexCoord0;
      gl_TexCoord[1] = gl_MultiTexCoord0.xxxy + vec4(    -dx,   0,  dx, -2.0*dy);  //  A1 B1 C1
      gl_TexCoord[2] = gl_MultiTexCoord0.xxxy + vec4(    -dx,   0,  dx,     -dy);  //   A  B  C
      gl_TexCoord[3] = gl_MultiTexCoord0.xxxy + vec4(    -dx,   0,  dx,       0);  //   D  E  F
      gl_TexCoord[4] = gl_MultiTexCoord0.xxxy + vec4(    -dx,   0,  dx,      dy);  //   G  H  I
      gl_TexCoord[5] = gl_MultiTexCoord0.xxxy + vec4(    -dx,   0,  dx,  2.0*dy);  //  G5 H5 I5
      gl_TexCoord[6] = gl_MultiTexCoord0.xyyy + vec4(-2.0*dx, -dy,   0,      dy);  //  A0 D0 G0
      gl_TexCoord[7] = gl_MultiTexCoord0.xyyy + vec4( 2.0*dx, -dy,   0,      dy);  //  C4 F4 I4
    }
fragment: |
    uniform sampler2D rubyTexture;
    uniform vec2 rubyTextureSize;

    const float coef = 2.0;

    const float threshold = 15.0;

    const float y_weight = 48.0;
    const float u_weight = 7.0;
    const float v_weight = 6.0;

    const mat3 yuv = mat3(0.299, 0.587, 0.114, -0.169, -0.331, 0.499, 0.499, -0.418, -0.0813);
    const mat3 yuv_weighted = mat3(y_weight * yuv[0], u_weight * yuv[1], v_weight * yuv[2]);

    vec4 RGBtoYUV(vec3 v0, vec3 v1, vec3 v2, vec3 v3) {
      float a = yuv_weighted[0].x * v0.x + yuv_weighted[0].y * v0.y + yuv_weighted[0].z * v0.z;
      float b = yuv_weighted[0].x * v1.x + yuv_weighted[0].y * v1.y + yuv_weighted[0].z * v1.z;
      float c = yuv_weighted[0].x * v2.x + yuv_weighted[0].y * v2.y + yuv_weighted[0].z * v2.z;
      float d = yuv_weighted[0].x * v3.x + yuv_weighted[0].y * v3.y + yuv_weighted[0].z * v3.z;

      return vec4(a, b, c, d);
    }

    bvec4 _and_(bvec4 A, bvec4 B) {
      return bvec4(A.x && B.x, A.y && B.y, A.z && B.z, A.w && B.w);
    }

    bvec4 _or_(bvec4 A, bvec4 B) {
      return bvec4(A.x || B.x, A.y || B.y, A.z || B.z, A.w || B.w);
    }

    vec4 df(vec4 A, vec4 B) {
      return vec4(abs(A - B));
    }

    bvec4 close(vec4 A, vec4 B) {
      return (lessThan(df(A, B), vec4(threshold)));
    }

    vec4 weighted_distance(vec4 a, vec4 b, vec4 c, vec4 d, vec4 e, vec4 f, vec4 g, vec4 h) {
      return (df(a, b) + df(a, c) + df(d, e) + df(d, f) + 4.0 * df(g, h));
    }

    void main() {
      vec2 fp = fract(gl_TexCoord[0].xy * rubyTextureSize);

      vec3 A1 = texture2D(rubyTexture, gl_TexCoord[1].xw).rgb;
      vec3 B1 = texture2D(rubyTexture, gl_TexCoord[1].yw).rgb;
      vec3 C1 = texture2D(rubyTexture, gl_TexCoord[1].zw).rgb;

      vec3 A  = texture2D(rubyTexture, gl_TexCoord[2].xw).rgb;
      vec3 B  = texture2D(rubyTexture, gl_TexCoord[2].yw).rgb;
      vec3 C  = texture2D(rubyTexture, gl_TexCoord[2].zw).rgb;

      vec3 D  = texture2D(rubyTexture, gl_TexCoord[3].xw).rgb;
      vec3 E  = texture2D(rubyTexture, gl_TexCoord[3].yw).rgb;
      vec3 F  = texture2D(rubyTexture, gl_TexCoord[3].zw).rgb;

      vec3 G  = texture2D(rubyTexture, gl_TexCoord[4].xw).rgb;
      vec3 H  = texture2D(rubyTexture, gl_TexCoord[4].yw).rgb;
      vec3 I  = texture2D(rubyTexture, gl_TexCoord[4].zw).rgb;

      vec3 G5 = texture2D(rubyTexture, gl_TexCoord[5].xw).rgb;
      vec3 H5 = texture2D(rubyTexture, gl_TexCoord[5].yw).rgb;
      vec3 I5 = texture2D(rubyTexture, gl_TexCoord[5].zw).rgb;

      vec3 A0 = texture2D(rubyTexture, gl_TexCoord[6].xy).rgb;
      vec3 D0 = texture2D(rubyTexture, gl_TexCoord[6].xz).rgb;
      vec3 G0 = texture2D(rubyTexture, gl_TexCoord[6].xw).rgb;

      vec3 C4 = texture2D(rubyTexture, gl_TexCoord[7].xy).rgb;
      vec3 F4 = texture2D(rubyTexture, gl_TexCoord[7].xz).rgb;
      vec3 I4 = texture2D(rubyTexture, gl_TexCoord[7].xw).rgb;

      vec4 b = RGBtoYUV(B, D, H, F);
      vec4 c = RGBtoYUV(C, A, G, I);
      vec4 e = RGBtoYUV(E, E, E, E);
      vec4 d = b.yzwx;
      vec4 f = b.wxyz;
      vec4 g = c.zwxy;
      vec4 h = b.zwxy;
      vec4 i = c.wxyz;

      vec4 i4 = RGBtoYUV(I4, C1, A0, G5);
      vec4 i5 = RGBtoYUV(I5, C4, A1, G0);
      vec4 h5 = RGBtoYUV(H5, F4, B1, D0);
      vec4 f4 = h5.yzwx;

      vec4 Ao = vec4( 1.0, -1.0, -1.0,  1.0 );
      vec4 Bo = vec4( 1.0,  1.0, -1.0, -1.0 );
      vec4 Co = vec4( 1.5,  0.5, -0.5,  0.5 );
      vec4 Ax = vec4( 1.0, -1.0, -1.0,  1.0 );
      vec4 Bx = vec4( 0.5,  2.0, -0.5, -2.0 );
      vec4 Cx = vec4( 1.0,  1.0, -0.5,  0.0 );
      vec4 Ay = vec4( 1.0, -1.0, -1.0,  1.0 );
      vec4 By = vec4( 2.0,  0.5, -2.0, -0.5 );
      vec4 Cy = vec4( 2.0,  0.0, -1.0,  0.5 );

      // These inequations define the line below which interpolation occurs
      bvec4 fx      = greaterThan(Ao * fp.y + Bo * fp.x, Co);
      bvec4 fx_left = greaterThan(Ax * fp.y + Bx * fp.x, Cx);
      bvec4 fx_up   = greaterThan(Ay * fp.y + By * fp.x, Cy);

      bvec4 t1 = _and_( notEqual(e, f), notEqual(e, h) );
      bvec4 t2 = _and_( not(close(f, b)), not(close(h, d)) );
      bvec4 t3 = _and_( _and_( close(e, i), not(close(f, i4)) ), not(close(h, i5)) );
      bvec4 t4 = _or_( close(e, g), close(e, c) );
      bvec4 interp_restriction_lv1 = _and_( t1, _or_( _or_(t2, t3), t4 ) );

      bvec4 interp_restriction_lv2_left = _and_( notEqual(e, g), notEqual(d, g) );
      bvec4 interp_restriction_lv2_up   = _and_( notEqual(e, c), notEqual(b, c) );

      bvec4 edr      = _and_( lessThan(weighted_distance(e, c, g, i, h5, f4, h, f),
                                       weighted_distance(h, d, i5, f, i4, b, e, i)), interp_restriction_lv1 );
      bvec4 edr_left = _and_( lessThanEqual(coef * df(f, g), df(h, c)), interp_restriction_lv2_left );
      bvec4 edr_up   = _and_( greaterThanEqual(df(f, g), coef * df(h, c)), interp_restriction_lv2_up );

      bvec4 nc = _and_( edr, _or_( _or_( fx, _and_(edr_left, fx_left) ), _and_(edr_up, fx_up) ) );

      bvec4 px = lessThanEqual(df(e, f), df(e, h));

      vec3 res = nc.x ? px.x ? F : H : nc.y ? px.y ? B : F : nc.z ? px.z ? D : B : nc.w ? px.w ? H : D : E;

      gl_FragColor.rgb = res;
      gl_FragColor.a = 1.0;
    }
linear: false