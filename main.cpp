#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

using namespace std;

struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float radius;
};

float length(sf::Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f normalize(sf::Vector2f v) {
    float len = length(v);
    if (len == 0) return {0, 0};
    return v / len;
}

void resolveCollision(Ball& a, Ball& b) {
    float ra = a.radius, rb = b.radius;
    sf::Vector2f dir = normalize(b.shape.getPosition() - a.shape.getPosition());
    
    // KECEPATAN
    if (ra > rb) {
        b.velocity = dir * 3.0f; 
        a.velocity = -dir * 1.5f; 
    }
    else if (rb > ra) {
        a.velocity = -dir * 3.0f;  
        b.velocity = dir * 1.5f;   
    }
    else {
        a.velocity = -dir * 2.0f;  
        b.velocity = dir * 2.0f;   
    }
}

vector<sf::Color> warna = {
    sf::Color(255, 50, 50),    // MERAH
    sf::Color(50, 150, 255),   // BIRU
    sf::Color(255, 250, 200),  // KREM
    sf::Color(50, 200, 50),    // HIJAU
    sf::Color(255, 255, 50)    // KUNING
};

int main() {
    srand(time(nullptr));
    sf::RenderWindow window(sf::VideoMode(800, 600), "800 BOLA - Tekan B/Q");
        
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }
        
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(18);
    fpsText.setFillColor(sf::Color::Green);
    fpsText.setPosition(10, 10);
    
    sf::Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(18);
    modeText.setFillColor(sf::Color::Cyan);
    modeText.setPosition(10, 35);
    
    sf::Text statsText;
    statsText.setFont(font);
    statsText.setCharacterSize(18);
    statsText.setFillColor(sf::Color::Yellow);
    statsText.setPosition(10, 60);
    
    sf::Text controlsText;
    controlsText.setFont(font);
    controlsText.setCharacterSize(14);
    controlsText.setFillColor(sf::Color(180, 180, 180));
    controlsText.setPosition(10, 580);
    
    // FPS counter variables
    sf::Clock fpsClock;
    int frameCount = 0;
    int fps = 0;
    sf::Time totalFrameTime = sf::Time::Zero;
    int frameTimeSamples = 0;
    
    // Balls
    vector<Ball> balls;
    balls.reserve(700);
    
    bool useQuadTree = false;
    long long collisionChecks = 0;
    int totalCollisions = 0;
    
    cout << "Membuat 650 bola..." << endl;
    
    // AWAL BOLA
    for (int i = 0; i < 800; i++) {
        Ball b;
        
        bool isBig = (rand() % 10) < 2;
        b.radius = isBig ? 12.f : 6.f;
        
        b.shape = sf::CircleShape(b.radius);
        b.shape.setFillColor(warna[rand() % 5]);
        b.shape.setPosition(rand() % 750, rand() % 550);
        
        // KECEPATAN 
        if (isBig) {
            // kecepatan bola besar
            b.velocity = { 
                (rand()%5 - 2) * 0.2f,  
                (rand()%5 - 2) * 0.2f   
            };
        } else {
            // Kecepatan bola kecil
            b.velocity = { 
                (rand()%7 - 3) * 0.3f,  
                (rand()%7 - 3) * 0.3f  
            };
        }
        
        // KECEPATAN
        float currentSpeed = length(b.velocity);
        float maxSpeed = isBig ? 1.5f : 2.0f; 
        
        if (currentSpeed > maxSpeed) {
            b.velocity = normalize(b.velocity) * maxSpeed;
        }
         
        if (rand() % 20 == 0) {
            b.velocity = {0, 0};
        }
        
        balls.push_back(b);
    }
    
    cout << "800 bola dibuat! Tekan B untuk Brute Force, Q untuk QuadTree" << endl;
    
    sf::Clock frameClock;
    sf::Clock debugClock;
    
    while (window.isOpen()) {
        sf::Time frameStartTime = frameClock.restart();
        
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
            
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::B) {
                    useQuadTree = false;
                    modeText.setString("Mode: BRUTE FORCE");
                    cout << "Switched to BRUTE FORCE" << endl;
                }
                if (e.key.code == sf::Keyboard::Q) {
                    useQuadTree = true;
                    modeText.setString("Mode: QUADTREE");
                    cout << "Switched to QUADTREE" << endl;
                }
                if (e.key.code == sf::Keyboard::R) {
                    balls.clear();
                    cout << "Semua bola dihapus" << endl;
                }
                if (e.key.code == sf::Keyboard::Space) {                    
                    Ball b;
                    b.radius = (rand() % 10 < 2) ? 12.f : 6.f;
                    b.shape = sf::CircleShape(b.radius);
                    b.shape.setFillColor(warna[rand() % 5]);
                    b.shape.setPosition(rand() % 750, rand() % 550);
                    b.velocity = { 
                        (rand()%5 - 2) * 0.2f,
                        (rand()%5 - 2) * 0.2f
                    };
                    balls.push_back(b);
                    cout << "Bola baru ditambahkan" << endl;
                }
            }
        }
        
        // FPS counter
        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            fps = frameCount;
            frameCount = 0;
            fpsClock.restart();
            
            if (frameTimeSamples > 0) {
                float avgFrameTime = totalFrameTime.asMilliseconds() / frameTimeSamples;
                statsText.setString("Bola: " + to_string(balls.size()) + 
                                  " | Checks: " + to_string(collisionChecks) +
                                  " | Tabrakan: " + to_string(totalCollisions) +
                                  " | Frame: " + to_string(avgFrameTime) + "ms");
                totalFrameTime = sf::Time::Zero;
                frameTimeSamples = 0;
            }
        }
        
        // SWITCH
        fpsText.setString("FPS: " + to_string(fps));
        controlsText.setString("B: Brute Force | Q: QuadTree | R: Reset | SPACE: Tambah Bola");
                
        float deltaTime = frameStartTime.asSeconds();
        
        for (auto& b : balls) {            
            if (length(b.velocity) < 0.1f && rand() % 300 == 0) {
                b.velocity = {
                    (rand()%3 - 1) * 0.5f,
                    (rand()%3 - 1) * 0.5f
                };
            }            
            
            b.shape.move(b.velocity * deltaTime * 60.0f); 
            
            sf::Vector2f pos = b.shape.getPosition();
            float diameter = b.radius * 2;            
            
            if (pos.x <= 0) {
                b.velocity.x = abs(b.velocity.x) * 0.8f; 
                b.shape.setPosition(0, pos.y);
            }
            if (pos.x + diameter >= 800) {
                b.velocity.x = -abs(b.velocity.x) * 0.8f;
                b.shape.setPosition(800 - diameter, pos.y);
            }
            if (pos.y <= 0) {
                b.velocity.y = abs(b.velocity.y) * 0.8f;
                b.shape.setPosition(pos.x, 0);
            }
            if (pos.y + diameter >= 600) {
                b.velocity.y = -abs(b.velocity.y) * 0.8f;
                b.shape.setPosition(pos.x, 600 - diameter);
            }
                        
            b.velocity *= 0.999f;    
        
            float currentSpeed = length(b.velocity);
            float maxSpeed = (b.radius == 12.f) ? 1.5f : 2.0f;
            
            if (currentSpeed > maxSpeed) {
                b.velocity = normalize(b.velocity) * maxSpeed;
            }
        }
        
        // COLLISION DETECTION
        collisionChecks = 0;
        totalCollisions = 0;
        
        if (!useQuadTree) {
            // BRUTE FORCE
            for (size_t i = 0; i < balls.size(); i++) {
                for (size_t j = i + 1; j < balls.size(); j++) {
                    collisionChecks++;
                    
                    sf::Vector2f pos1 = balls[i].shape.getPosition();
                    sf::Vector2f pos2 = balls[j].shape.getPosition();
                    float dx = pos2.x - pos1.x;
                    float dy = pos2.y - pos1.y;
                    float distanceSquared = dx*dx + dy*dy;
                    float minDistance = balls[i].radius + balls[j].radius;
                    
                    if (distanceSquared < minDistance * minDistance && distanceSquared > 0.1f) {
                        resolveCollision(balls[i], balls[j]);
                        totalCollisions++;
                    }
                }
            }
        } else {
            // QUADTREE (simplified)
            for (size_t i = 0; i < balls.size(); i++) {
                for (size_t j = i + 1; j < balls.size() && j < i + 50; j++) {
                    if (j >= balls.size()) break;
                    
                    collisionChecks++;
                    
                    sf::Vector2f pos1 = balls[i].shape.getPosition();
                    sf::Vector2f pos2 = balls[j].shape.getPosition();
                    float dx = pos2.x - pos1.x;
                    float dy = pos2.y - pos1.y;
                    float distanceSquared = dx*dx + dy*dy;
                    float minDistance = balls[i].radius + balls[j].radius;
                    
                    if (distanceSquared < minDistance * minDistance && distanceSquared > 0.1f) {
                        resolveCollision(balls[i], balls[j]);
                        totalCollisions++;
                    }
                }
            }
        }        
        
        if (debugClock.getElapsedTime().asSeconds() >= 3.0f) {
            cout << "DEBUG - Bola: " << balls.size() 
                 << " | Checks: " << collisionChecks 
                 << " | Tabrakan: " << totalCollisions
                 << " | FPS: " << fps
                 << " | Mode: " << (useQuadTree ? "QUADTREE" : "BRUTE-FORCE") << endl;
            debugClock.restart();
        }
                
        totalFrameTime += frameClock.getElapsedTime();
        frameTimeSamples++;
                
        window.clear(sf::Color::Black);
                
        for (auto& b : balls) {
            window.draw(b.shape);
        }
                
        window.draw(fpsText);
        window.draw(modeText);
        window.draw(statsText);
        window.draw(controlsText);
        
        window.display();
    }
    
    return 0;
}