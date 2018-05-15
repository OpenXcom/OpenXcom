#<!--
#   SABR XCOMified v1.3 Shader
#
#   Copyright (C) 2011, 2012 Hyllian/Jararaca - sergiogdb@gmail.com (5xBR)
#   Copyright (C) 2012 crazy46guy (GLSL conversion)
#   Copyright (C) 2012 Joshua Street (SABR)
#   Copyright (c) 2014 Daemonjax (XCOMification)
#
#   Portions of this algorithm were taken from Hyllian's 5xBR v3.7c
#   shader and Joshua Street's SABR v3.0 shader.
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#-->

language: "GLSL"
vertex: |
    uniform vec2 rubyTextureSize;

    varying vec2 tc;
    varying vec4 xyp_e1_e2_e3;
    varying vec4 xyp_f1_f2_f3;
    varying vec4 xyp_g1_g2_g3;
    varying vec4 xyp_h1_h2_h3;
    varying vec4 xyp_a_b_c_d;
    varying vec4 xyp_1_2_3;
    varying vec4 xyp_5_10_15;
    varying vec4 xyp_6_7_8;
    varying vec4 xyp_9_14_9;
    varying vec4 xyp_11_12_13;
    varying vec4 xyp_16_17_18;
    varying vec4 xyp_21_22_23;

    void main() {
      gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

      float x = 1.0 / rubyTextureSize.x;
      float y = 1.0 / rubyTextureSize.y;

      /* Mask for algorhithm
         -4x  -3x  -2x   -x    0    x   2x   3x   4x
        +----+----+----+----+----+----+----+----+----+
             |    |    |    |    |    |    |    |    | -4y
        +----+----+----+----+----+----+----+----+----+
             |    |    | e1 | e2 | e3 |    |    |    | -3y
        +----+----+----+----+----+----+----+----+----+
             |    |  a |  1 |  2 |  3 |  b |    |    | -2y
        +----+----+----+----+----+----+----+----+----+
             | h1 |  5 |  6 |  7 |  8 |  9 | f1 |    |  -y
        +----+----+----+----+----+----+----+----+----+
             | h2 | 10 | 11 | 12 | 13 | 14 | f2 |    |   0
        +----+----+----+----+----+----+----+----+----+
             | h3 | 15 | 16 | 17 | 18 | 19 | f3 |    |   y
        +----+----+----+----+----+----+----+----+----+
             |    |  c | 21 | 22 | 23 |  d |    |    |  2y
        +----+----+----+----+----+----+----+----+----+
             |    |    | g1 | g2 | g3 |    |    |    |  3y
        +----+----+----+----+----+----+----+----+----+
             |    |    |    |    |    |    |    |    |  4y
        +----+----+----+----+----+----+----+----+----+
      */

      tc = gl_MultiTexCoord0.xy;

      /*
      xyp_e1_e2_e3 = tc.xxxy + vec4(      -x, 0.0,   x, -3.0 * y);
      xyp_f1_f2_f3 = tc.xyyy + vec4( 3.0 * x,  -y, 0.0,        y);
      xyp_g1_g2_g3 = tc.xxxy + vec4(      -x, 0.0,   x,  3.0 * y);
      xyp_h1_h2_h3 = tc.xyyy + vec4(-3.0 * x,  -y, 0.0,        y);
      xyp_a_b_c_d  = tc.xxyy + vec4(-2.0 * x, 2.0  * x, -2.0 * y, 2.0 * y);
      */
      xyp_1_2_3    = tc.xxxy + vec4(      -x, 0.0,   x, -2.0 * y);
      xyp_6_7_8    = tc.xxxy + vec4(      -x, 0.0,   x,       -y);
      xyp_11_12_13 = tc.xxxy + vec4(      -x, 0.0,   x,      0.0);
      xyp_16_17_18 = tc.xxxy + vec4(      -x, 0.0,   x,        y);
      xyp_21_22_23 = tc.xxxy + vec4(      -x, 0.0,   x,  2.0 * y);
      xyp_5_10_15  = tc.xyyy + vec4(-2.0 * x,  -y, 0.0,        y);
      xyp_9_14_9   = tc.xyyy + vec4( 2.0 * x,  -y, 0.0,        y);
    }
linear: false
fragment: |
    /* Uniforms
      - rubyTexture: texture sampler
      - rubyTextureSize: size of the texture before rendering
    */
    uniform sampler2D rubyTexture;
    uniform vec2 rubyTextureSize;

    /* Varying attributes
      - tc: coordinate of the texel being processed
      - xyp_[]_[]_[]: a packed coordinate for 3 or 4 areas within the texture
    */


    varying vec2 tc;
    /*
    varying vec4 xyp_e1_e2_e3;
    varying vec4 xyp_f1_f2_f3;
    varying vec4 xyp_g1_g2_g3;
    varying vec4 xyp_h1_h2_h3;
    varying vec4 xyp_a_b_c_d;
    */
    varying vec4 xyp_1_2_3;
    varying vec4 xyp_5_10_15;
    varying vec4 xyp_6_7_8;
    varying vec4 xyp_9_14_9;
    varying vec4 xyp_11_12_13;
    varying vec4 xyp_16_17_18;
    varying vec4 xyp_21_22_23;

    /* Constants
    */

    // Pallete colors used in exclusion rules
     const vec3 ex1_Yellow = vec3(252.0, 252.0,   0.0) / 255.0;
     const vec3 ex1_Red1   = vec3(192.0,   0.0,   0.0) / 255.0;
     const vec3 ex1_Red2   = vec3(252.0,   0.0,   0.0) / 255.0;
     const vec3 ex1_Cyan1  = vec3(0.0  , 192.0, 192.0) / 255.0;
     const vec3 ex1_Cyan2  = vec3(0.0  , 252.0, 252.0) / 255.0;
     const vec3 ex_Pink1   = vec3(252.0, 164.0, 212.0) / 255.0;
     const vec3 ex_Pink2   = vec3(192.0, 112.0, 152.0) / 255.0;

    // Inequation coefficients for interpolation
    //    Equations are in the form: Ay + Bx = C
    //    45, 30, and 60 denote the angle from x each line the cooeficient variable set builds

    const vec4 Ai  = vec4(1.0, -1.0, -1.0,  1.0);
    const vec4 B45 = vec4(1.0,  1.0, -1.0, -1.0);
    const vec4 C45 = vec4(1.5,  0.5, -0.5,  0.5);
    const vec4 B30 = vec4(0.5,  2.0, -0.5, -2.0);
    const vec4 C30 = vec4(1.0,  1.0, -0.5,  0.0);
    const vec4 B60 = vec4(2.0,  0.5, -2.0, -0.5);
    const vec4 C60 = vec4(2.0,  0.0, -1.0,  0.5);

    const vec4 M45 = vec4(0.4,  0.4,  0.4,  0.4);
    const vec4 M30 = vec4(0.2,  0.4,  0.2,  0.4);
    const vec4 M60 = M30.yxwz;
    const vec4 Mshift = vec4(0.2);

    const float coef = 2.0; // Coefficient for weighted edge detection

    const vec4 threshold = vec4(43.34); // Threshold for if luminance values are "equal"

    const float colorMax = 255.0; // pallete colors only seem to have an effective RGB range of [0 - 252.0], should we use 252 or 255 as mult for lum?
    // Conversion from RGB to Luminance (most accurate way I found so far)
    const vec3 lum = vec3(0.241, 0.691, 0.068) * colorMax * colorMax; // openxcom uses original color pallete converted to RGB color, which is then converted to percentage [0.0 - 1.0]

    /* Helper functions
    */


    bvec4 _and_(bvec4 A, bvec4 B) { // Performs same logic operation as && for vectors
      return bvec4(A.x && B.x, A.y && B.y, A.z && B.z, A.w && B.w);
    }

    bvec4 _or_(bvec4 A, bvec4 B) { // Performs same logic operation as || for vectors
      return bvec4(A.x || B.x, A.y || B.y, A.z || B.z, A.w || B.w);
    }

    float get_lum(vec3 v) { // returns approximated luminance value of a color
      vec3 temp = vec3(v.x * v.x * lum.x, v.y * v.y * lum.y, v.z * v.z * lum.z);
      return sqrt(temp.x + temp.y + temp.z);
    }

    vec4 RGBtoLum4(vec3 v0, vec3 v1, vec3 v2, vec3 v3) { // returns approximated luminance value of 4 colors
      return vec4(get_lum(v0), get_lum(v1), get_lum(v2), get_lum(v3));
    }

    vec4 getLumDifference(vec4 A, vec4 B) { // Gets the difference between 2 4-value luminance vectors
      return abs(A - B);
    }

    bvec4 lumClose2(vec4 A, vec4 B) { // Determines if 2 4-value luminance vectors are "equal" based on threshold
      return lessThan(getLumDifference(A, B), threshold);
    }

    vec4 lum_wd(vec4 a, vec4 b, vec4 c, vec4 d, vec4 e, vec4 f, vec4 g, vec4 h) {
      return getLumDifference(a, b) + getLumDifference(a, c) + getLumDifference(d, e) + getLumDifference(d, f) + 4.0 * getLumDifference(g, h);
    }

    bool colorsEqual2(vec3 A, vec3 B) { //returns true if the two RGB color vectors are equal
         return all(equal(A, B));
    }

    bool colors3_eq(vec3 A, vec3 B, vec3 C) { //returns true if the four RGB color vectors are equal
        return colorsEqual2(A, B) && colorsEqual2(B, C);
    }

    bool colorsEqual4(vec3 A, vec3 B, vec3 C, vec3 D) {  //returns true if the four RGB color vectors are equal
        return colors3_eq(A, B, C) && colorsEqual2(C, D);
    }

    //bool colorsEqual5(vec3 A, vec3 B, vec3 C, vec3 D, vec3 E) { //returns true if the four RGB color vectors are equal
    //    return colors3_eq(A, B, C) && colors3_eq(C, D, E);
    //}

    float colors2_Diff(vec3 c1, vec3 c2) { // Gets the difference between 2 3-value rgb colors
      vec3 df = abs(c1 - c2);
      return df.r + df.g + df.b;
    }

    void main() {
      // Get mask values by performing texture lookup with the uniform sampler

      /*
      vec3 Ph1 = texture2D(rubyTexture, xyp_h1_h2_h3.xy).rgb;
      vec3 Ph2 = texture2D(rubyTexture, xyp_h1_h2_h3.xz).rgb;
      vec3 Ph3 = texture2D(rubyTexture, xyp_h1_h2_h3.xw).rgb;

      vec3 Pg1 = texture2D(rubyTexture, xyp_g1_g2_g3.xw).rgb;
      vec3 Pg2 = texture2D(rubyTexture, xyp_g1_g2_g3.yw).rgb;
      vec3 Pg3 = texture2D(rubyTexture, xyp_g1_g2_g3.zw).rgb;

      vec3 Pf1 = texture2D(rubyTexture, xyp_f1_f2_f3.xy).rgb;
      vec3 Pf2 = texture2D(rubyTexture, xyp_f1_f2_f3.xz).rgb;
      vec3 Pf3 = texture2D(rubyTexture, xyp_f1_f2_f3.xw).rgb;

      vec3 Pe1 = texture2D(rubyTexture, xyp_e1_e2_e3.xw).rgb;
      vec3 Pe2 = texture2D(rubyTexture, xyp_e1_e2_e3.yw).rgb;
      vec3 Pe3 = texture2D(rubyTexture, xyp_e1_e2_e3.zw).rgb;

      vec3 Pa  = texture2D(rubyTexture, xyp_a_b_c_d.xz ).rgb;
      vec3 Pb  = texture2D(rubyTexture, xyp_a_b_c_d.yz ).rgb;
      vec3 Pc  = texture2D(rubyTexture, xyp_a_b_c_d.xw ).rgb;
      vec3 Pd  = texture2D(rubyTexture, xyp_a_b_c_d.yw ).rgb;
      */
      vec3 P1  = texture2D(rubyTexture, xyp_1_2_3.xw   ).rgb;
      vec3 P2  = texture2D(rubyTexture, xyp_1_2_3.yw   ).rgb;
      vec3 P3  = texture2D(rubyTexture, xyp_1_2_3.zw   ).rgb;

      vec3 P6  = texture2D(rubyTexture, xyp_6_7_8.xw   ).rgb;
      vec3 P7  = texture2D(rubyTexture, xyp_6_7_8.yw   ).rgb;
      vec3 P8  = texture2D(rubyTexture, xyp_6_7_8.zw   ).rgb;

      vec3 P11 = texture2D(rubyTexture, xyp_11_12_13.xw).rgb;
      vec3 P12 = texture2D(rubyTexture, xyp_11_12_13.yw).rgb;
      vec3 P13 = texture2D(rubyTexture, xyp_11_12_13.zw).rgb;

      vec3 P16 = texture2D(rubyTexture, xyp_16_17_18.xw).rgb;
      vec3 P17 = texture2D(rubyTexture, xyp_16_17_18.yw).rgb;
      vec3 P18 = texture2D(rubyTexture, xyp_16_17_18.zw).rgb;

      vec3 P21 = texture2D(rubyTexture, xyp_21_22_23.xw).rgb;
      vec3 P22 = texture2D(rubyTexture, xyp_21_22_23.yw).rgb;
      vec3 P23 = texture2D(rubyTexture, xyp_21_22_23.zw).rgb;

      vec3 P5  = texture2D(rubyTexture, xyp_5_10_15.xy ).rgb;
      vec3 P10 = texture2D(rubyTexture, xyp_5_10_15.xz ).rgb;
      vec3 P15 = texture2D(rubyTexture, xyp_5_10_15.xw ).rgb;

      vec3 P9  = texture2D(rubyTexture, xyp_9_14_9.xy  ).rgb;
      vec3 P14 = texture2D(rubyTexture, xyp_9_14_9.xz  ).rgb;
      vec3 P19 = texture2D(rubyTexture, xyp_9_14_9.xw  ).rgb;

      /* Mask for algorhithm
         -4x  -3x  -2x   -x    0    x   2x   3x   4x
        +----+----+----+----+----+----+----+----+----+
             |    |    |    |    |    |    |    |    | -4y
        +----+----+----+----+----+----+----+----+----+
             |    |    | e1 | e2 | e3 |    |    |    | -3y
        +----+----+----+----+----+----+----+----+----+
             |    |  a |  1 |  2 |  3 |  b |    |    | -2y
        +----+----+----+----+----+----+----+----+----+
             | h1 |  5 |  6 |  7 |  8 |  9 | f1 |    |  -y
        +----+----+----+----+----+----+----+----+----+
             | h2 | 10 | 11 | 12 | 13 | 14 | f2 |    |   0
        +----+----+----+----+----+----+----+----+----+
             | h3 | 15 | 16 | 17 | 18 | 19 | f3 |    |   y
        +----+----+----+----+----+----+----+----+----+
             |    |  c | 21 | 22 | 23 |  d |    |    |  2y
        +----+----+----+----+----+----+----+----+----+
             |    |    | g1 | g2 | g3 |    |    |    |  3y
        +----+----+----+----+----+----+----+----+----+
             |    |    |    |    |    |    |    |    |  4y
        +----+----+----+----+----+----+----+----+----+
      */

      // Store luminance values of each point in groups of 4
      // so that we may operate on all four corners at once
      vec4 p7  = RGBtoLum4(P7,  P11, P17, P13);   // 45 - tight - 7 11 17 13
      vec4 p8  = RGBtoLum4(P8,  P6,  P16, P18);   // 90 - tight - 8  6 16 18
      vec4 p11 = p7.yzwx;                      // 45 - tight - 11 17 13 7 - duplicate
      vec4 p12 = RGBtoLum4(P12, P12, P12, P12);   //  0 - center
      vec4 p13 = p7.wxyz;                      // 45 - tight - 13 7 11 17 - duplicate
      vec4 p14 = RGBtoLum4(P14, P2,  P10, P22);   // 45 - wide  - 14 2 10 22
      vec4 p16 = p8.zwxy;                      // 90 - tight - 16 18 8  6 - duplicate
      vec4 p17 = p7.zwxy;                      // 45 - tight - 17 13 7 11 - duplicate
      vec4 p18 = p8.wxyz;                      // 90 - tight - 18  8 6 16 - duplicate
      vec4 p19 = RGBtoLum4(P19, P3,  P5,  P21);   // 36 - wide  - 19  3 5 21
      vec4 p22 = p14.wxyz;                     // 45 - wide  - 22 14 2 10 - duplicate
      vec4 p23 = RGBtoLum4(P23, P9,  P1,  P15);   // 36 - wide  - 23 9  1 15

      // Scale current texel coordinate to [0..1]
      vec2 fp = fract(tc * rubyTextureSize);

      // Determine amount of "smoothing" or mixing that could be done on texel corners
      vec4 ma45 = smoothstep(C45 - M45, C45 + M45, Ai * fp.y + B45 * fp.x);
      vec4 ma30 = smoothstep(C30 - M30, C30 + M30, Ai * fp.y + B30 * fp.x);
      vec4 ma60 = smoothstep(C60 - M60, C60 + M60, Ai * fp.y + B60 * fp.x);
      vec4 marn = smoothstep(C45 - M45 + Mshift, C45 + M45 + Mshift, Ai * fp.y + B45 * fp.x);

      // Perform edge weight calculations
      vec4 e45   = lum_wd(p12, p8, p16, p18, p22, p14, p17, p13);
      vec4 econt = lum_wd(p17, p11, p23, p13, p7, p19, p12, p18);
      vec4 e30   = getLumDifference(p13, p16);
      vec4 e60   = getLumDifference(p8, p17);

      // Calculate rule results for interpolation
      bvec4 r45_1   = _and_(notEqual(p12, p13), notEqual(p12, p17));
      bvec4 r45_2   = _and_(not(lumClose2(p13, p7)), not(lumClose2(p13, p8)));
      bvec4 r45_3   = _and_(not(lumClose2(p17, p11)), not(lumClose2(p17, p16)));
      bvec4 r45_4_1 = _and_(not(lumClose2(p13, p14)), not(lumClose2(p13, p19)));
      bvec4 r45_4_2 = _and_(not(lumClose2(p17, p22)), not(lumClose2(p17, p23)));
      bvec4 r45_4   = _and_(lumClose2(p12, p18), _or_(r45_4_1, r45_4_2));
      bvec4 r45_5   = _or_(lumClose2(p12, p16), lumClose2(p12, p8));
      bvec4 r45     = _and_(r45_1, _or_(_or_(_or_(r45_2, r45_3), r45_4), r45_5));
      bvec4 r30     = _and_(notEqual(p12, p16), notEqual(p11, p16));
      bvec4 r60     = _and_(notEqual(p12, p8), notEqual(p7, p8));

      // Combine rules with edge weights
      bvec4 edr45 = _and_(lessThan(e45, econt), r45);
      bvec4 edrrn = lessThanEqual(e45, econt);
      bvec4 edr30 = _and_(lessThanEqual(coef * e30, e60), r30);
      bvec4 edr60 = _and_(lessThanEqual(coef * e60, e30), r60);

      // Finalize interpolation rules and cast to float (0.0 for false, 1.0 for true)
      vec4 final45 = vec4(_and_(_and_(not(edr30), not(edr60)), edr45));
      vec4 final30 = vec4(_and_(_and_(edr45, not(edr60)), edr30));
      vec4 final60 = vec4(_and_(_and_(edr45, not(edr30)), edr60));
      vec4 final36 = vec4(_and_(_and_(edr60, edr30), edr45));
      vec4 finalrn = vec4(_and_(not(edr45), edrrn));

      // Determine the color to mix with for each corner
      vec4 px = step(getLumDifference(p12, p17), getLumDifference(p12, p13));

      /* Mask for algorhithm
         -4x  -3x  -2x   -x    0    x   2x   3x   4x
        +----+----+----+----+----+----+----+----+----+
             |    |    |    |    |    |    |    |    | -4y
        +----+----+----+----+----+----+----+----+----+
             |    |    |Pe1 |Pe2 |Pe3 |    |    |    | -3y
        +----+----+----+----+----+----+----+----+----+
             |    | Pa | P1 | P2 | P3 | Pb |    |    | -2y
        +----+----+----+----+----+----+----+----+----+
             |Ph1 | P5 | P6 | P7 | P8 | P9 |Pf1 |    |  -y
        +----+----+----+----+----+----+----+----+----+
             |Ph2 |P10 |P11 |P12 |P13 |P14 |Pf2 |    |   0
        +----+----+----+----+----+----+----+----+----+
             |Ph3 |P15 |P16 |P17 |P18 |P19 |Pf3 |    |   y
        +----+----+----+----+----+----+----+----+----+
             |    | Pc |P21 |P22 |P23 | Pd |    |    |  2y
        +----+----+----+----+----+----+----+----+----+
             |    |    |Pg1 |Pg2 |Pg3 |    |    |    |  3y
        +----+----+----+----+----+----+----+----+----+
             |    |    |    |    |    |    |    |    |  4y
        +----+----+----+----+----+----+----+----+----+
      */

      // XCom Excuclsion Rule 1 = Geoscape: cities, bases, terror sites, ufos

      bool exclusionRule = !(((all(equal(P6 , ex1_Cyan1))  || all(equal(P6 , ex1_Cyan2))) && any(notEqual(P12, P6)) && (colorsEqual4(P6, P8, P16, P18))) || // Gooey base center
                              (all(equal(P12, ex1_Yellow)) && all(equal(P7, ex1_Red1))    &&  all(equal(P7, P17)))  || // juicy yellow center
                              (all(equal(P12, ex1_Red1))   || all(equal(P12, ex1_Red2)))  ||   // all red (cities, terror sites, ufos)
                              (all(equal(P17, ex1_Red1))   || all(equal(P17, ex1_Red2)))  || // above red top
                              (all(equal(P7 , ex1_Red1))   || all(equal(P7, ex1_Red2)))   || // below red bottom (
                              (all(equal(P13, ex1_Red1))   || all(equal(P13, ex1_Red2)))  || // left of red
                              (all(equal(P11, ex1_Red1))   || all(equal(P11, ex1_Red2)))  || // right of red
                              (all(equal(P12, ex1_Cyan1))  || all(equal(P12, ex1_Cyan2))) || // base
                              (all(equal(P11, ex_Pink1))   || all(equal(P11, ex_Pink2))   || all(equal(P13 , ex_Pink1)) || all(equal(P13, ex_Pink2))) || // terror site pink (all around)
                              (all(equal(P7 , ex_Pink1))   || all(equal(P7 , ex_Pink2))   || all(equal(P17 , ex_Pink1)) || all(equal(P17, ex_Pink2)))
                            );

      // Determine the mix amounts by combining the final rule result and correspondingix amount for the rule in each corner
      vec4 mac = (final36 * max(ma30, ma60) + final30 * ma30 + final60 * ma60 + final45 * ma45 + finalrn * marn) * float(exclusionRule);

      /* Calculate the resulting color by traversing clockwise and counter-clockwise around the corners of the texel.
         Finally choose the result that has the largest difference from the texel's original color.
      */
      vec3 res1 = P12;
      res1 = mix(res1, mix(P13, P17, px.x), mac.x);
      res1 = mix(res1, mix(P7, P13, px.y), mac.y);
      res1 = mix(res1, mix(P11, P7, px.z), mac.z);
      res1 = mix(res1, mix(P17, P11, px.w), mac.w);

      vec3 res2 = P12;
      res2 = mix(res2, mix(P17, P11, px.w), mac.w);
      res2 = mix(res2, mix(P11, P7, px.z), mac.z);
      res2 = mix(res2, mix(P7, P13, px.y), mac.y);
      res2 = mix(res2, mix(P13, P17, px.x), mac.x);

      gl_FragColor = vec4(vec3(mix(res1, res2, step(colors2_Diff(P12, res1), colors2_Diff(P12, res2)))), 1.0);
    }
