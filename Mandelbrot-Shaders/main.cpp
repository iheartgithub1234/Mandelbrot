#include <SFML/Graphics.hpp>
#include <iostream>

// Window settings
const int WIDTH = 800;
const int HEIGHT = 800;

// Mandelbrot parameters
double centerX = -0.75, centerY = 0.0;
double scale = 4.0;
const int maxIterations = 300; // Fixed iterations

int main() {
    // Create SFML Window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot (GPU-Accelerated)");

    // Load Shader
    sf::Shader mandelbrotShader;
    if (!mandelbrotShader.loadFromFile("mandelbrot.frag", sf::Shader::Fragment)) {
        std::cerr << "Error loading shader!\n";
        return -1;
    }

    // Fullscreen Quad for Shader Rendering
    sf::RectangleShape screen(sf::Vector2f(WIDTH, HEIGHT));
    screen.setPosition(0, 0);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                double zoomFactor = (event.mouseButton.button == sf::Mouse::Left) ? 0.5 : 2.0;

                // Convert mouse coordinates to Mandelbrot coordinates BEFORE zooming
                double mouseReal = centerX + (event.mouseButton.x - WIDTH / 2.0) * (scale / WIDTH);
                double mouseImag = centerY + (event.mouseButton.y - HEIGHT / 2.0) * (scale / HEIGHT);

                // Apply zoom
                scale *= zoomFactor;

                // Recalculate the center based on the zoom and mouse position
                centerX = mouseReal + (centerX - mouseReal) * zoomFactor;
                centerY = mouseImag + (centerY - mouseImag) * zoomFactor;
            }

            // Move with Arrow Keys (correcting up/down direction)
            // if (event.type == sf::Event::KeyPressed) {
            //    double moveAmount = scale * 0.1;
            //    if (event.key.code == sf::Keyboard::Left) centerX -= moveAmount;
            //    if (event.key.code == sf::Keyboard::Right) centerX += moveAmount;
            //    if (event.key.code == sf::Keyboard::Up) centerY += moveAmount;   // Invert direction
             //   if (event.key.code == sf::Keyboard::Down) centerY -= moveAmount; // Invert direction
            //}
        }

        // Update shader parameters
        mandelbrotShader.setUniform("resolution", sf::Vector2f(WIDTH, HEIGHT));
        mandelbrotShader.setUniform("center", sf::Vector2f(centerX, centerY));
        mandelbrotShader.setUniform("scale", static_cast<float>(scale));
        mandelbrotShader.setUniform("maxIterations", maxIterations);

        // Draw using the shader
        window.clear();
        window.draw(screen, &mandelbrotShader);
        window.display();
    }

    return 0;
}
