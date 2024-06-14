#include <iostream>
#include <math.h>
#include <vector>
#include <chrono>
#include <SFML/Graphics.hpp>

#define STEADY_CLOCK std::chrono::steady_clock

#define WIDTH 1920
#define HEIGHT 1080
#define RECTWIDTH 64
#define RECTHEIGHT 36
#define MARGIN 8
#define BLOCKDELAY 1000
#define SHOOTDELAY 500
#define CANHEIGHT 60
#define CANWIDTH 30


struct block;
struct projectile;

static void display(), manageEvent(sf::Event event), addBlock(int x, int y, int health), setcolor(block *curblock, int health), delay(int ms), shoot(), hitblock(int rectindex, int shellindex);

class block{
    private:
        int points;
    public:
        sf::RectangleShape shape;
        int health;
        block(sf::RectangleShape shape, int health) : shape(shape), health(health){
            points = health;
        }
        sf::Vector2f midpos(){
            return(sf::Vector2f(this->shape.getPosition().x + RECTWIDTH/2, this->shape.getPosition().y + RECTHEIGHT/2));
        }
        int getPoints(){
            return points;
        }
};

class projectile{
    private:
        int speed;
    public:
        sf::RectangleShape shape;
        int bounces;
        float rotation;
        projectile(sf::RectangleShape shape, int speed, int bounces, float rotation) : shape(shape), speed(speed), bounces(bounces), rotation(rotation){}
        int getSpeed(){
            return speed;
        }
};

sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "block breaker", sf::Style::Fullscreen);
std::vector<block> rects;
std::vector<projectile> bullets;
sf::RectangleShape cannon = sf::RectangleShape(sf::Vector2f(CANWIDTH, CANHEIGHT)); //30, 60
int fps;


bool leftclicked = false, doneonce = false;

int main(){
    std::srand(std::clock());
    STEADY_CLOCK::time_point falltimer = STEADY_CLOCK::now();
    STEADY_CLOCK::time_point shoottimer = STEADY_CLOCK::now();

    cannon.setOrigin(15, CANHEIGHT);
    cannon.setPosition(WIDTH/2, HEIGHT);
    bool movedown = false;

    while (window.isOpen()){
        STEADY_CLOCK::time_point fpstimer = STEADY_CLOCK::now();
        display();
        sf::Event event;
        manageEvent(event);

        for(int i = 0; i < rects.size(); i ++){
            if(movedown){
               rects[i].shape.move(0, RECTHEIGHT+MARGIN);
            }
        }
        for(int i = 0; i < bullets.size(); i++){
            float speed = bullets[i].getSpeed() / (float)fps;
            std::cout << "speed: " << speed << std::endl;
            bullets[i].shape.move(speed * (sinf32(bullets[i].rotation * (M_PI/180))), -speed * (cosf32((float)bullets[i].rotation * (M_PI/180))));
        }
        movedown = false;

        if(std::chrono::duration<float, std::milli>(STEADY_CLOCK::now() - falltimer).count() > BLOCKDELAY){
            movedown = true;
            addBlock((std::rand() % (WIDTH/RECTWIDTH)*RECTWIDTH), -RECTHEIGHT, 1);
            falltimer = STEADY_CLOCK::now();
        }
        cannon.setRotation(-(atan2((WIDTH/2)-sf::Mouse::getPosition(window).x, (HEIGHT-CANHEIGHT)-sf::Mouse::getPosition(window).y) * 180 / M_PI));

        if(leftclicked && std::chrono::duration<float, std::milli>(STEADY_CLOCK::now() - shoottimer).count() > SHOOTDELAY){
            shoot();
            shoottimer = STEADY_CLOCK::now();
        }

        leftclicked = false;
        fps = std::chrono::duration<float, std::milli>(STEADY_CLOCK::now() - fpstimer).count() * 1000;
        std::cout << "fps based on frame time: " << fps << std::endl;
    }
}

static void display(){
    window.clear();
    for(int i = 0; i < rects.size(); i++){
        window.draw(rects[i].shape);
    }
    for(int i = 0; i < bullets.size(); i++){
        window.draw(bullets[i].shape);
    }
    window.draw(cannon);
    window.display();
}


static void manageEvent(sf::Event event){
    while (window.pollEvent(event)){
        if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
            exit(0);
        }
    }

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        leftclicked = true;
    }
}

static void addBlock(int x, int y, int health){
    block newblock(sf::RectangleShape(sf::Vector2f(RECTWIDTH-MARGIN, RECTHEIGHT-MARGIN)), health);
    newblock.shape.setPosition(sf::Vector2f(x, y));
    setcolor(&newblock, health);
    rects.push_back(newblock);
}

static void setcolor(block *curblock, int health){
    const sf::Color hcolours[6] = {sf::Color::Red, sf::Color::Blue, sf::Color(255, 128, 0, 255), sf::Color::Yellow, sf::Color::Green, sf::Color::Magenta};
    if (health < 6){
        curblock->shape.setFillColor(hcolours[0]);
    }
    else if (health < 15){
        curblock->shape.setFillColor(hcolours[1]);
    }
    else if (health < 25){
        curblock->shape.setFillColor(hcolours[2]);
    }
    else if (health < 50){
        curblock->shape.setFillColor(hcolours[3]);
    }
    else if (health < 100){
        curblock->shape.setFillColor(hcolours[4]);
    }
    else{
        curblock->shape.setFillColor(hcolours[5]);
    }
}

static void delay(int ms){
    STEADY_CLOCK::time_point starttime = STEADY_CLOCK::now();
    while (std::chrono::duration<float, std::milli>(STEADY_CLOCK::now() - starttime).count() < ms){}
}

static void shoot(){
    projectile newbullet(sf::RectangleShape(sf::Vector2f(20, 20)), 30, 1, cannon.getRotation());
    newbullet.shape.setOrigin(10, 10);
    newbullet.shape.setPosition(CANHEIGHT*(sinf32((float)cannon.getRotation() * (M_PI/180))) + cannon.getPosition().x, cannon.getPosition().y - CANHEIGHT*(cosf32((float)cannon.getRotation() * (M_PI/180))));
    bullets.push_back(newbullet);
}

static void hitblock(int rectindex, int shellindex){
    rects.erase(rects.begin());
    bullets.erase(bullets.begin());
}