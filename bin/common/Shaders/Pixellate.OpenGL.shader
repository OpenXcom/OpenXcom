language: "GLSL"
vertex: |
  void main() {
    gl_Position    = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
  }
linear: false
fragment: |
  uniform sampler2D rubyTexture;
  uniform vec2      rubyTextureSize;

  void main() {
    vec2 texelSize = 1.0 / rubyTextureSize;

    vec2 range;
    range.x = dFdx(gl_TexCoord[0].x) / 2.0 * 0.99;
    range.y = dFdy(gl_TexCoord[0].y) / 2.0 * 0.99;

    float left   = gl_TexCoord[0].x - range.x;
    float top    = gl_TexCoord[0].y + range.y;
    float right  = gl_TexCoord[0].x + range.x;
    float bottom = gl_TexCoord[0].y - range.y;

    vec4 topLeftColor     = texture2D(rubyTexture, (floor(vec2(left, top)     / texelSize) + 0.5) * texelSize);
    vec4 bottomRightColor = texture2D(rubyTexture, (floor(vec2(right, bottom) / texelSize) + 0.5) * texelSize);
    vec4 bottomLeftColor  = texture2D(rubyTexture, (floor(vec2(left, bottom)  / texelSize) + 0.5) * texelSize);
    vec4 topRightColor    = texture2D(rubyTexture, (floor(vec2(right, top)    / texelSize) + 0.5) * texelSize);

    vec2 border = clamp(round(gl_TexCoord[0] / texelSize) * texelSize, vec2(left, bottom), vec2(right, top));

    float totalArea = 4.0 * range.x * range.y;

    vec4 averageColor;
    averageColor  = ((border.x - left)  * (top - border.y)    / totalArea) * topLeftColor;
    averageColor += ((right - border.x) * (border.y - bottom) / totalArea) * bottomRightColor;
    averageColor += ((border.x - left)  * (border.y - bottom) / totalArea) * bottomLeftColor;
    averageColor += ((right - border.x) * (top - border.y)    / totalArea) * topRightColor;

    gl_FragColor = averageColor;
  }
