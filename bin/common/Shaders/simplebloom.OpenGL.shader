#<!--
#WhateverMan's Bloom Shader
#Copyright (c) 2010 WhateverMan
#
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#as published by the Free Software Foundation; either version 2
#of the License, or (at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#-->
language: "GLSL"
vertex: |
      void main()
      {
         gl_Position    = ftransform();
         gl_TexCoord[0] = gl_MultiTexCoord0;
      }
linear: false
fragment: |
      uniform sampler2D texture;
      #define glarebasesize 0.896
      #define power 0.65 // 0.50 is good

      uniform vec2 rubyTextureSize;

      void main()
      {
         vec4 sum = vec4(0.0);
         vec4 bum = vec4(0.0);
         vec2 texcoord = vec2(gl_TexCoord[0]);
         int j;
         int i;

         vec2 glaresize = vec2(glarebasesize) / rubyTextureSize;

         for(i = -2; i < 2; i++)
         {
            for (j = -1; j < 1; j++)
            {
               sum += texture2D(texture, texcoord + vec2(-i, j)*glaresize) * power;
               bum += texture2D(texture, texcoord + vec2(j, i)*glaresize) * power;
            }
         }

         if (texture2D(texture, texcoord).r < 2.0)
         {
            gl_FragColor = sum*sum*sum*0.001+bum*bum*bum*0.0080 + texture2D(texture, texcoord);
         }
      }
