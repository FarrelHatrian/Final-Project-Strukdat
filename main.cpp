#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

float distance(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Bola Mantul + Tabrakan + Mouse");
    window.setFramerateLimit(60);

    std::srand(std::time(nullptr));
    std::vector<Ball> balls;

    sf::Color colors[3] = {
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue
    };

    // Bola awal
    for (int i = 0; i < 10; i++) {
        Ball b;
        b.shape = sf::CircleShape(10.f);
        b.shape.setFillColor(colors[std::rand() % 3]);
        b.shape.setPosition(std::rand() % 780, std::rand() % 580);
        b.velocity = sf::Vector2f(
            (std::rand() % 5 + 2) * (std::rand() % 2 ? 1 : -1),
            (std::rand() % 5 + 2) * (std::rand() % 2 ? 1 : -1)
        );
        balls.push_back(b);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // KLIK MOUSE = TAMBAH BOLA
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {

                Ball b;
                b.shape = sf::CircleShape(10.f);
                b.shape.setFillColor(colors[std::rand() % 3]);
                b.shape.setPosition(event.mouseButton.x, event.mouseButton.y);
                b.velocity = sf::Vector2f(
                    (std::rand() % 5 + 2) * (std::rand() % 2 ? 1 : -1),
                    (std::rand() % 5 + 2) * (std::rand() % 2 ? 1 : -1)
                );
                balls.push_back(b);
            }
        }

        // GERAK + PANTUL DINDING
        for (auto& b : balls) {
            b.shape.move(b.velocity);

            if (b.shape.getPosition().x <= 0 ||
                b.shape.getPosition().x + b.shape.getRadius() * 2 >= 800)
                b.velocity.x *= -1;

            if (b.shape.getPosition().y <= 0 ||
                b.shape.getPosition().y + b.shape.getRadius() * 2 >= 600)
                b.velocity.y *= -1;
        }

        // TABRAKAN ANTAR BOLA
        for (size_t i = 0; i < balls.size(); i++) {
            for (size_t j = i + 1; j < balls.size(); j++) {
                sf::Vector2f posA = balls[i].shape.getPosition();
                sf::Vector2f posB = balls[j].shape.getPosition();

                float r = balls[i].shape.getRadius() * 2;
                if (distance(posA, posB) < r) {
                    std::swap(balls[i].velocity, balls[j].velocity);
                }
            }
        }

        window.clear(sf::Color::Black);
        for (auto& b : balls)
            window.draw(b.shape);
        window.display();
    }
}
