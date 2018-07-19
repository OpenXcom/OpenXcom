# <?xml version="1.0" encoding="UTF-8"?>
# <!--
    # Scanline 3x Shader
    # Copyright (C) 2011 hunterk

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

# This shader works best at scale 3x or else the pixels don't match up correctly.
# -->
language: "GLSL"
vertex: |
    void main(void) {
      gl_Position = ftransform();
      gl_TexCoord[0] = gl_MultiTexCoord0;
    }
fragment: |
    uniform sampler2D rubyTexture;

    void main(void) {
      vec4 rgb = texture2D(rubyTexture, gl_TexCoord[0].xy);
      vec4 intens ;
      if (fract(gl_FragCoord.y * (0.5*4.0/3.0)) > 0.5)
        intens = vec4(0);
        else
        intens = smoothstep(0.2,0.8,rgb) + normalize(vec4(rgb.xyz, 1.0));
        float level = (4.0-gl_TexCoord[0].z) * 0.19;
      gl_FragColor = intens * (0.5-level) + rgb * 1.1 ;
    }
linear: false