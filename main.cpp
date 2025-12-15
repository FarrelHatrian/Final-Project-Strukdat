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

struct Rect {
    float x, y, width, height;
    bool contains(Ball* b) {
        sf::Vector2f pos = b->shape.getPosition();
        return (pos.x >= x && pos.x <= x + width &&
                pos.y >= y && pos.y <= y + height);
    }
    bool intersects(const Rect& other) {
        return !(other.x > x + width || other.x + other.width < x ||
                 other.y > y + height || other.y + other.height < y);
    }
};

class QuadTree {
    Rect boundary;
    size_t capacity;
    vector<Ball*> balls;
    bool divided;
    QuadTree *nw, *ne, *sw, *se;

public:
    QuadTree(Rect boundary, size_t capacity)
        : boundary(boundary), capacity(capacity), divided(false),
          nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr) {}

    ~QuadTree() {
        delete nw; delete ne; delete sw; delete se;
    }

    void subdivide() {
        float x = boundary.x;
        float y = boundary.y;
        float w = boundary.width / 2;
        float h = boundary.height / 2;
        nw = new QuadTree({x, y, w, h}, capacity);
        ne = new QuadTree({x + w, y, w, h}, capacity);
        sw = new QuadTree({x, y + h, w, h}, capacity);
        se = new QuadTree({x + w, y + h, w, h}, capacity);
        divided = true;
    }

    bool insert(Ball* b) {
        if (!boundary.contains(b)) return false;
        if (balls.size() < capacity) {
            balls.push_back(b);
            return true;
        } else {
            if (!divided) subdivide();
            if (nw->insert(b)) return true;
            if (ne->insert(b)) return true;
            if (sw->insert(b)) return true;
            if (se->insert(b)) return true;
        }
        return false;
    }

    void query(Rect range, vector<Ball*>& found) {
        if (!boundary.intersects(range)) return;
        for (auto b : balls)
            if (range.contains(b)) found.push_back(b);
        if (divided) {
            nw->query(range, found);
            ne->query(range, found);
            sw->query(range, found);
            se->query(range, found);
        }
    }
};

vector<sf::Color> warna = {
    sf::Color(255, 50, 50),
    sf::Color(50, 150, 255),
    sf::Color(255, 250, 200),
    sf::Color(50, 200, 50),
    sf::Color(255, 255, 50)
};

int main() {
    srand(time(nullptr));
    sf::RenderWindow window(sf::VideoMode(800, 600), "Final Project");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }

    sf::Text fpsText, modeText, statsText, controlsText;
    fpsText.setFont(font); fpsText.setCharacterSize(18); fpsText.setFillColor(sf::Color::Green); fpsText.setPosition(10,10);
    modeText.setFont(font); modeText.setCharacterSize(18); modeText.setFillColor(sf::Color::Cyan); modeText.setPosition(10,35);
    statsText.setFont(font); statsText.setCharacterSize(18); statsText.setFillColor(sf::Color::Yellow); statsText.setPosition(10,60);
    controlsText.setFont(font); controlsText.setCharacterSize(14); controlsText.setFillColor(sf::Color(180,180,180)); controlsText.setPosition(10,580);

    sf::Clock fpsClock, frameClock, debugClock;
    int frameCount = 0, fps = 0, frameTimeSamples = 0;
    sf::Time totalFrameTime = sf::Time::Zero;

    vector<Ball> balls;
    balls.reserve(700);

    bool useQuadTree = false;
    long long collisionChecks = 0;
    int totalCollisions = 0;

    // Buat bola
    for (int i=0;i<3000;i++){
        Ball b;
        bool isBig = (rand()%10 < 2);
        b.radius = isBig?12.f:6.f;
        b.shape = sf::CircleShape(b.radius);
        b.shape.setFillColor(warna[rand()%5]);
        b.shape.setPosition(rand()%750, rand()%550);
        b.velocity = isBig ? sf::Vector2f((rand()%5-2)*0.2f,(rand()%5-2)*0.2f) :
                             sf::Vector2f((rand()%7-3)*0.3f,(rand()%7-3)*0.3f);
        float maxSpeed = isBig?1.5f:2.0f;
        if (length(b.velocity)>maxSpeed) b.velocity = normalize(b.velocity)*maxSpeed;
        if (rand()%20==0) b.velocity={0,0};
        balls.push_back(b);
    }

    while(window.isOpen()){
        sf::Time frameStartTime = frameClock.restart();
        sf::Event e;
        while(window.pollEvent(e)){
            if(e.type==sf::Event::Closed) window.close();
            if(e.type==sf::Event::KeyPressed){
                if(e.key.code==sf::Keyboard::B){ useQuadTree=false; modeText.setString("Mode: BRUTE FORCE"); }
                if(e.key.code==sf::Keyboard::Q){ useQuadTree=true; modeText.setString("Mode: QUADTREE"); }
                if(e.key.code==sf::Keyboard::R){ balls.clear(); }
                if(e.key.code==sf::Keyboard::Space){
                    Ball b; b.radius=(rand()%10<2)?12.f:6.f; b.shape=sf::CircleShape(b.radius);
                    b.shape.setFillColor(warna[rand()%5]);
                    b.shape.setPosition(rand()%750, rand()%550);
                    b.velocity={ (rand()%5-2)*0.2f,(rand()%5-2)*0.2f};
                    balls.push_back(b);
                }
            }
        }

        // FPS
        frameCount++;
        if(fpsClock.getElapsedTime().asSeconds()>=1.0f){
            fps=frameCount; frameCount=0; fpsClock.restart();
            if(frameTimeSamples>0){
                float avgFrameTime=totalFrameTime.asMilliseconds()/frameTimeSamples;
                
        
                statsText.setString("Bola:"+to_string(balls.size()));

                
                
                totalFrameTime=sf::Time::Zero; frameTimeSamples=0;
            }
        }

        fpsText.setString("FPS: "+to_string(fps));
        controlsText.setString("B: Brute Force | Q: QuadTree | R: Reset | SPACE: Tambah Bola");

        float deltaTime = frameStartTime.asSeconds();
        for(auto& b: balls){
            if(length(b.velocity)<0.1f && rand()%300==0) b.velocity={ (rand()%3-1)*0.5f,(rand()%3-1)*0.5f };
            b.shape.move(b.velocity*deltaTime*60.f);
            sf::Vector2f pos=b.shape.getPosition(); float diameter=b.radius*2;
            if(pos.x<=0){ b.velocity.x=abs(b.velocity.x)*0.8f; b.shape.setPosition(0,pos.y);}
            if(pos.x+diameter>=800){ b.velocity.x=-abs(b.velocity.x)*0.8f; b.shape.setPosition(800-diameter,pos.y);}
            if(pos.y<=0){ b.velocity.y=abs(b.velocity.y)*0.8f; b.shape.setPosition(pos.x,0);}
            if(pos.y+diameter>=600){ b.velocity.y=-abs(b.velocity.y)*0.8f; b.shape.setPosition(pos.x,600-diameter);}
            b.velocity*=0.999f;
            float maxSpeed=(b.radius==12.f)?1.5f:2.0f;
            if(length(b.velocity)>maxSpeed) b.velocity=normalize(b.velocity)*maxSpeed;
        }

        collisionChecks=0; totalCollisions=0;

        if(!useQuadTree){
            // Brute Force
            for(size_t i=0;i<balls.size();i++){
                for(size_t j=i+1;j<balls.size();j++){
                    collisionChecks++;
                    sf::Vector2f delta=balls[j].shape.getPosition()-balls[i].shape.getPosition();
                    float dist2=delta.x*delta.x+delta.y*delta.y;
                    float minDist=balls[i].radius+balls[j].radius;
                    if(dist2<minDist*minDist && dist2>0.01f){
                        resolveCollision(balls[i],balls[j]); totalCollisions++;
                    }
                }
            }
        } else {
            // QuadTree
            QuadTree qt({0,0,800,600},4);
            for(auto& b:balls) qt.insert(&b);
            for(auto& b:balls){
                Rect range{ b.shape.getPosition().x - b.radius, b.shape.getPosition().y - b.radius, b.radius*2, b.radius*2 };
                vector<Ball*> others;
                qt.query(range, others);
                for(auto& other:others){
                    if(&b==other) continue;
                    collisionChecks++;
                    sf::Vector2f delta=other->shape.getPosition()-b.shape.getPosition();
                    float dist2=delta.x*delta.x+delta.y*delta.y;
                    float minDist=b.radius+other->radius;
                    if(dist2<minDist*minDist && dist2>0.01f){
                        resolveCollision(b,*other); totalCollisions++;
                    }
                }
            }
        }

        if(debugClock.getElapsedTime().asSeconds()>=3.0f){
            cout<<"DEBUG - Bola: "<<balls.size()<<" | Checks: "<<collisionChecks
                <<" | Tabrakan: "<<totalCollisions<<" | FPS: "<<fps
                <<" | Mode: "<<(useQuadTree?"QUADTREE":"BRUTE-FORCE")<<endl;
            debugClock.restart();
        }

        totalFrameTime+=frameClock.getElapsedTime();
        frameTimeSamples++;

        window.clear(sf::Color::Black);
        for(auto& b:balls) window.draw(b.shape);
        window.draw(fpsText); window.draw(modeText); window.draw(statsText); window.draw(controlsText);
        window.display();
    }

    return 0;
}