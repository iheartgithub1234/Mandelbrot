#version 330 core
uniform vec2 resolution;
uniform vec2 center;
uniform float scale;
uniform int maxIterations;

out vec4 fragColor;

void main() {
    // Flip the y-coordinate to match Mandelbrot space
    vec2 flippedCoord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);

    // Convert pixel coordinates to Mandelbrot space
    vec2 c = center + (flippedCoord - resolution / 2.0) * (scale / resolution.x);
    vec2 z = vec2(0.0);

    int iter;
    for (iter = 0; iter < maxIterations; iter++) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (2.0 * z.x * z.y) + c.y;
        z = vec2(x, y);
        
        if (dot(z, z) > 4.0) break;
    }

    // Mandelbrot set points (inside the set) remain black
    if (iter == maxIterations) {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // **Change Here: Stretch colors outward**
    float t = pow(float(iter) / float(maxIterations), 0.5);

    // Color mapping remains the same
    float r = 0.5 + 0.5 * cos(6.28318 * (t + 0.0)); 
    float g = 0.5 + 0.5 * cos(6.28318 * (t + 0.33));
    float b = 0.5 + 0.5 * cos(6.28318 * (t + 0.67));

    fragColor = vec4(r, g, b, 1.0);
}
