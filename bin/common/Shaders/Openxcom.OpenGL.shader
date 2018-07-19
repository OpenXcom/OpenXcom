#<!--
#   Standard Openxcom shader file
#
#   Copyright (C) 2013 - Openxcom Project - https://openxcom.org/
#
#   AUTHOR: Powerc80
#
#   DESCRIPTION: This shader is designed to smooth rough edges, while
#   maintaining a semi-pixellated feel similar to the original X-com.
#   This code was based guest(r)'s 4xGLSLHqFilter OpenGL shader.
#
#   =======================LEGAL INFORMATION=========================
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 3
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   For more information --> http://www.gnu.org/licenses/gpl.html
#
#   =================================================================
#
#-->
language: "GLSL"
vertex: |
        #version 110
        uniform vec2 rubyTextureSize;

        void main()
        {
                //////////////////////////////////////////////////
                //X_VALUE and Y_VALUE: these values refer to the//
                //length & width of the view-window rectangle   //
                //(i.e. the x and y dimensions of the screen). A//
                //value of 0.0001 produces a result close to the//
                //original resolution of X-com, while a value of//
                //0.001 produces a slightly smoother screen.    //
                //Values higher than 0.002 are not recommended. //
                //////////////////////////////////////////////////
                const float x_value = 0.00077;
                const float y_value = 0.00077;

                //////////////////////////////////////////////////
                //Here we define several vertices that are used //
                //as an overlay rectangle for the screens/menus.//
                //                                              //
                //Where tr = top-right vertex                   //
                //      tl = top-left vertex                    //
                //      br = bottom-right vertex                //
                //      bl = bottom-left vertex                 //
                //////////////////////////////////////////////////
                const vec2 tr = vec2(x_value,y_value);
                const vec2 tl = vec2(-x_value,y_value);
                const vec2 br = vec2(x_value,0.0);
                const vec2 bl = vec2(-x_value,0.0);

                ////////////////////////////////////////////////////////
                //Defines screen position and initial texture coord's.//
                ////////////////////////////////////////////////////////
                gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                gl_TexCoord[0] = gl_MultiTexCoord0;

                //////////////////////////////////////////////////////
                //Defines the x-y section of the texture coordinate.//
                //////////////////////////////////////////////////////
                gl_TexCoord[1].xy = gl_TexCoord[0].xy - (tr * 0.55);
                gl_TexCoord[2].xy = gl_TexCoord[0].xy - (tl * 0.55);
                gl_TexCoord[3].xy = gl_TexCoord[0].xy + (tr * 0.55);
                gl_TexCoord[4].xy = gl_TexCoord[0].xy + (tl * 0.55);
                gl_TexCoord[5].xy = gl_TexCoord[0].xy - tr;
                gl_TexCoord[6].xy = gl_TexCoord[0].xy + tr;

                //////////////////////////////////////////////////////
                //Defines the z-w section of the texture coordinate.//
                //////////////////////////////////////////////////////
                gl_TexCoord[1].zw = gl_TexCoord[0].xy - bl;
                gl_TexCoord[2].zw = gl_TexCoord[0].xy + br;
                gl_TexCoord[3].zw = gl_TexCoord[0].xy + bl;
                gl_TexCoord[4].zw = gl_TexCoord[0].xy - br;
                gl_TexCoord[5].zw = gl_TexCoord[0].xy - tl;
                gl_TexCoord[6].zw = gl_TexCoord[0].xy + tl;
        }
fragment: |
        #version 110
        uniform sampler2D rubyTexture;

        ///////////////////////////////////////////////////////////
        //Initial Vertex: the first value of the texture coord's,//
        //which is used as a 'centre' for all the other values.  //
        ///////////////////////////////////////////////////////////
        vec3 centre = texture2D(rubyTexture, gl_TexCoord[0].xy).xyz;

        ///////////////////////////////////////////////////////////////
        //Reference Vertex: used later when calculating dot-products.//
        ///////////////////////////////////////////////////////////////
        const vec3 ref = vec3(1.0,1.0,1.0);

        //////////////////////////////////////////////////////////////
        //WEIGHT VARIABLES: Several variables are declared here     //
        //to be used to precisely define the various filter weights.//
        //                                                          //
        //Where start_weight = starting weight                      //
        //                 k = weight decreasing factor             //
        //        max_weight = maximum filter weight                //
        //        min_weight = minimum filter weight                //
        //////////////////////////////////////////////////////////////
        const float start_weight =  1.04;
        const float k            = -1.07;
        const float max_weight   =  0.73;
        const float min_weight   =  0.05;

        ////////////////////////////////////////////////////////////////////
        //DP FUNCTION: Returns the adjusted dot-product of a given vertex.//
        ////////////////////////////////////////////////////////////////////
        float dp(vec3 v) {return dot(abs(v-centre),ref);}

        void main()
        {
              /////////////////////////////////////////////////////////
              //Attaches the defined coordinates of gl_TextCoord     //
              //to sampler2D to allow for modification via filtering.//
              /////////////////////////////////////////////////////////
              vec3 i1 = texture2D(rubyTexture, gl_TexCoord[1].xy).xyz;
              vec3 i2 = texture2D(rubyTexture, gl_TexCoord[2].xy).xyz;
              vec3 i3 = texture2D(rubyTexture, gl_TexCoord[3].xy).xyz;
              vec3 i4 = texture2D(rubyTexture, gl_TexCoord[4].xy).xyz;
              vec3 o1 = texture2D(rubyTexture, gl_TexCoord[5].xy).xyz;
              vec3 o3 = texture2D(rubyTexture, gl_TexCoord[6].xy).xyz;
              vec3 o2 = texture2D(rubyTexture, gl_TexCoord[5].zw).xyz;
              vec3 o4 = texture2D(rubyTexture, gl_TexCoord[6].zw).xyz;
              vec3 s1 = texture2D(rubyTexture, gl_TexCoord[1].zw).xyz;
              vec3 s2 = texture2D(rubyTexture, gl_TexCoord[2].zw).xyz;
              vec3 s3 = texture2D(rubyTexture, gl_TexCoord[3].zw).xyz;
              vec3 s4 = texture2D(rubyTexture, gl_TexCoord[4].zw).xyz;

              ////////////////////////////////////////////////////////////
              //FILTER WEIGHTS: These are the weights which will be used//
              //by the filter to produce the intended result.           //
              ////////////////////////////////////////////////////////////
              float w1 = min(dot(abs(i2-i4),ref), max(dp(o2),dp(o4)));
              float w2 = min(dot(abs(i1-i3),ref), max(dp(o1),dp(o3)));
              float w3 = min(dot(abs(i2-i4),ref), max(dp(o2),dp(o4)));
              float w4 = min(dot(abs(i1-i3),ref), max(dp(o1),dp(o3)));

              //////////////////////////////////////////////////////////////////
              //Adjusts the weight values, based on the previous dot-products.//
              //////////////////////////////////////////////////////////////////
              if (dp(o3) < dp(o1)) w1 *= dp(o3) / dp(o1);
              if (dp(o4) < dp(o2)) w2 *= dp(o4) / dp(o2);
              if (dp(o1) < dp(o3)) w3 *= dp(o1) / dp(o3);
              if (dp(o2) < dp(o4)) w4 *= dp(o2) / dp(o4);

              //////////////////////////////////////////////////////////////
              //Adjusts the value of centre, based on the current weights.//
              //////////////////////////////////////////////////////////////
              centre = (w1*o1+w2*o2+w3*o3+w4*o4+0.00077*centre) / (w1+w2+w3+w4+0.00077);

              /////////////////////////////////////////////////////////////////
              //The weights are then adjusted based on the decreasing factor,//
              //k, and the effects smoothing factor, FX_add.                 //
              /////////////////////////////////////////////////////////////////
              w1 = k*dot(abs(i1-centre)+abs(i3-centre),ref) / (0.125*dot(i1+i3,ref));
              w2 = k*dot(abs(i2-centre)+abs(i4-centre),ref) / (0.125*dot(i2+i4,ref));
              w3 = k*dot(abs(s1-centre)+abs(s3-centre),ref) / (0.125*dot(s1+s3,ref));
              w4 = k*dot(abs(s2-centre)+abs(s4-centre),ref) / (0.125*dot(s2+s4,ref));

              ////////////////////////////////////////////////////////////////////
              //Weights w1 to w4 are clamped between the min and the max weight,//
              //preventing them from aversely being too high or too low.        //
              ////////////////////////////////////////////////////////////////////
              w1 = clamp(w1+start_weight, min_weight, max_weight);
              w2 = clamp(w2+start_weight, min_weight, max_weight);
              w3 = clamp(w3+start_weight, min_weight, max_weight);
              w4 = clamp(w4+start_weight, min_weight, max_weight);

              ///////////////////////////////////////////////////////////////////////////
              //Applies the weights to filter the screen, and then displays the result.//
              ///////////////////////////////////////////////////////////////////////////
              gl_FragColor.xyz = (w1*(i1+i3)+w2*(i2+i4)+w3*(s1+s3)+w4*(s2+s4)+centre) / (2.0*(w1+w2+w3+w4)+1.0);
              gl_FragColor.a   = 1.0;
        }
linear: false
