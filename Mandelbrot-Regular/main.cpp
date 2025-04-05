#include <SFML/Graphics.hpp>
#include <complex>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>




const int WIDTH = 800;
const int HEIGHT = 600;
const int NUM_THREADS = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;


int qualityFactor = 1;


enum ColorPreset {
   WHITE_ON_BLACK,
   REVERSE,
   RAINBOW,
   FIRE,
   BLUE_GREEN
};


ColorPreset currentPreset = WHITE_ON_BLACK;


sf::Color getColorForIterations(int iterations, int maxIterations) {
   float ratio = float(iterations) / maxIterations;
   switch (currentPreset) {
       case WHITE_ON_BLACK:
           return (iterations == maxIterations) ? sf::Color::Black : sf::Color(255 * ratio, 255 * ratio, 255 * ratio);


       case REVERSE:
           return (iterations == maxIterations) ? sf::Color::Black : sf::Color(255 - int(255 * ratio), 255 - int(255 * ratio), 255 - int(255 * ratio));


         case RAINBOW:
         if (iterations == maxIterations) return sf::Color::Black;


         // Smooth transition: Red -> Orange -> Yellow -> Green -> Blue -> Dark Blue
         if (ratio < 0.2) {
                   // Red to Orange
                   return sf::Color(255, 165 * (ratio / 0.2), 0);
              } else if (ratio < 0.4) {
                   // Orange to Yellow
                   return sf::Color(255, 165 + 90 * ((ratio - 0.2) / 0.2), 0);
              } else if (ratio < 0.6) {
                   // Yellow to Green
                   return sf::Color(255 - 255 * ((ratio - 0.4) / 0.2), 255, 0);
              } else if (ratio < 0.8) {
                   // Green to Blue
                   return sf::Color(0, 255 - 205 * ((ratio - 0.6) / 0.2), 255 * ((ratio - 0.6) / 0.2));
              } else if (ratio < 1.0) {
                   // Blue to Dark Blue
                   return sf::Color(0, 50, 255 - 127 * ((ratio - 0.8) / 0.2));
              } else {
                   return sf::Color(0, 0, 0); // Black (background)
         }


         case FIRE:
           if (iterations == maxIterations) return sf::Color::Black;
           if (ratio < 0.1)
               return sf::Color(255 * (ratio * 2), 0, 0);
           else if (ratio < 0.15)
               return sf::Color(255 * (ratio * 3), 0, 0);
           else if (ratio < 0.3)
               return sf::Color(255 * (0.15 * 3), 165 * ratio, 0);
           else
               return sf::Color(255 * (0.2 * 4), 255 * ratio, 0);


       case BLUE_GREEN:
           if (iterations == maxIterations) return sf::Color::Black;
           if (ratio < 0.1)
               return sf::Color(0, 0, 255 * (ratio * 2));
           else if (ratio < 0.15)
               return sf::Color(0, 0, 255 * (ratio * 3));
           else if (ratio < 0.3)
               return sf::Color(0, 140 * ratio, 255 * (0.15 * 3));
           else
               return sf::Color(0, 220 * ratio, 255 * (0.2 * 4));


       default:
           return sf::Color::Black;
   }
}


void computeMandelbrot(sf::Image &image, double zoom, double offsetX, double offsetY, int maxIterations, int startY, int endY, int quality) {
   for (int x = 0; x < WIDTH; x += quality) {
       for (int y = startY; y < endY; y += quality) {
           std::complex<double> point((x - WIDTH / 2.0) * zoom + offsetX, (y - HEIGHT / 2.0) * zoom + offsetY);
           std::complex<double> z = point;
           int iterations = 0;
           while (std::abs(z) < 2.0 && iterations < maxIterations) {
               z = z * z + point;
               ++iterations;
           }
           sf::Color color = getColorForIterations(iterations, maxIterations);
           for (int dx = 0; dx < quality; ++dx) {
               for (int dy = 0; dy < quality; ++dy) {
                   if (x + dx < WIDTH && y + dy < HEIGHT) {
                       image.setPixel(x + dx, y + dy, color);
                   }
               }
           }
       }
   }
}


void drawMandelbrot(sf::Image &image, double zoom, double offsetX, double offsetY, int maxIterations, int quality) {
   auto start = std::chrono::high_resolution_clock::now();
   std::vector<std::thread> threads;
   int step = HEIGHT / NUM_THREADS;
   for (int i = 0; i < NUM_THREADS; ++i) {
       int startY = i * step;
       int endY = (i == NUM_THREADS - 1) ? HEIGHT : (i + 1) * step;
       threads.emplace_back(computeMandelbrot, std::ref(image), zoom, offsetX, offsetY, maxIterations, startY, endY, quality);
   }
   for (auto &t : threads) {
       t.join();
   }
   auto end = std::chrono::high_resolution_clock::now();
   double renderTime = std::chrono::duration<double>(end - start).count();
   std::cout << "\nRENDER TIME: " << renderTime << " seconds" << std::endl;
   std::cout << "CURRENT ITERATIONS: " << maxIterations << std::endl;
   std::cout << "CURRENT QUALITY: " << quality << "\n" << std::endl;
}


int main() {
   sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set");
   sf::Image image;
   image.create(WIDTH, HEIGHT, sf::Color::Black);
   sf::Texture texture;
   sf::Sprite sprite;


   double zoom = 0.005;
   double offsetX = -0.5;
   double offsetY = 0;
   int maxIterations = 100;


   drawMandelbrot(image, zoom, offsetX, offsetY, maxIterations, qualityFactor);
   texture.loadFromImage(image);
   sprite.setTexture(texture);


   while (window.isOpen()) {
       sf::Event event;
       while (window.pollEvent(event)) {
           if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
               window.close();
           } else if (event.type == sf::Event::MouseButtonPressed) {
               if (event.mouseButton.button == sf::Mouse::Left) {
                   offsetX += (event.mouseButton.x - WIDTH / 2.0) * zoom;
                   offsetY += (event.mouseButton.y - HEIGHT / 2.0) * zoom;
                   zoom *= 0.5;
               } else if (event.mouseButton.button == sf::Mouse::Right) {
                   offsetX += (event.mouseButton.x - WIDTH / 2.0) * zoom;
                   offsetY += (event.mouseButton.y - HEIGHT / 2.0) * zoom;
                   zoom *= 2.0;
               }
               drawMandelbrot(image, zoom, offsetX, offsetY, maxIterations, qualityFactor);
               texture.loadFromImage(image);
               sprite.setTexture(texture);
           } else if (event.type == sf::Event::KeyPressed) {
               if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9) {
                   if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                       qualityFactor = (event.key.code - sf::Keyboard::Num1 + 1);
                   } else {
                       maxIterations = (event.key.code - sf::Keyboard::Num1 + 1) * 100;
                   }
                   drawMandelbrot(image, zoom, offsetX, offsetY, maxIterations, qualityFactor);
                   texture.loadFromImage(image);
                   sprite.setTexture(texture);
               } else if (event.key.code == sf::Keyboard::Num0) {
                   zoom = 0.005;
                   offsetX = -0.5;
                   offsetY = 0;
                   maxIterations = 100;
                   qualityFactor = 1;
                   drawMandelbrot(image, zoom, offsetX, offsetY, maxIterations, qualityFactor);
                   texture.loadFromImage(image);
                   sprite.setTexture(texture);
               } else if (event.key.code == sf::Keyboard::Space) {
                   currentPreset = static_cast<ColorPreset>((static_cast<int>(currentPreset) + 1) % 5);
                   drawMandelbrot(image, zoom, offsetX, offsetY, maxIterations, qualityFactor);
                   texture.loadFromImage(image);
                   sprite.setTexture(texture);
               }
           }
       }
       window.clear();
       window.draw(sprite);
       window.display();
   }
   return 0;
}
