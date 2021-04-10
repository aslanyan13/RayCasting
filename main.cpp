#include <SFML/Graphics.hpp>

#include <iostream>
#include <Windows.h>
#include <vector>
#include <chrono>
#include <algorithm>
#include <math.h>
#include <time.h>

using namespace std;
using namespace sf;

const float PI = 3.14159f;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int MAP_HEIGHT = 20;
const int MAP_WIDTH = 20;

float graphicQuality = 3;

float playerX = 3.0f, playerY = 2.0f, playerA = 0;

float fOVCoff = 3.0f;
float fov = PI / fOVCoff;
float depth = 32.0f;

int main(void) {

    wstring map;

    // # - wall
    // . - free cell
    map += L"####################";
    map += L"#..................#";
    map += L"#.......#..........#";
    map += L"################.###";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"#........##....#...#";
    map += L"#........##....#...#";
    map += L"#..................#";
    map += L"#..................#";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"#....#.#############";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"####################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "RayCasting!", sf::Style::Default, settings);

    Texture wall;
    wall.loadFromFile("Textures/wall4.png");
    wall.setRepeated(true);

    float textureHeight = wall.getSize().y;
    float textureWidth = wall.getSize().y;

    Sprite wallSprite(wall);

    // Game loop
    while (window.isOpen()) {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> time = tp2 - tp1;
        tp1 = tp2;

        float elaspedTime = time.count();

        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();

        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            playerA -= 2.0f * elaspedTime;
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            playerA += 2.0f * elaspedTime;
        }

        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            playerX += sinf(playerA) * 5.0f * elaspedTime;
            playerY += cosf(playerA) * 5.0f * elaspedTime;

            if (map[(int)playerY * MAP_WIDTH + (int)playerX] == '#')
            {
                playerX -= sinf(playerA) * 5.0f * elaspedTime;
                playerY -= cosf(playerA) * 5.0f * elaspedTime;
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Down)) {
            playerX -= sinf(playerA) * 5.0f * elaspedTime;
            playerY -= cosf(playerA) * 5.0f * elaspedTime;

            if (map[(int)playerY * MAP_WIDTH + (int)playerX] == '#')
            {
                playerX += sinf(playerA) * 5.0f * elaspedTime;
                playerY += cosf(playerA) * 5.0f * elaspedTime;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Subtract))
        {
            graphicQuality--;
            if (graphicQuality < 1) graphicQuality = 1;
        }
        if (Keyboard::isKeyPressed(Keyboard::Add))
        {
            graphicQuality++;
        }

        cout << "Quality - " << graphicQuality << endl;

        RectangleShape floorShape;
        floorShape.setSize(Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2));
        floorShape.setFillColor(Color(50, 50, 50));
        floorShape.setPosition(0, SCREEN_HEIGHT / 2);

        window.draw(floorShape);

        for (int i = 0; i < SCREEN_WIDTH; i += graphicQuality)
        {
            float rayAngle = (playerA - fov / 2.0f) + ((float)i / (float)SCREEN_WIDTH) * fov;

            float distanceToWall = 0;
            bool hitWall = false;

            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);

            float testX, testY;

            while (!hitWall && distanceToWall < depth) {
                distanceToWall += 0.01;

                testX = (playerX + eyeX * distanceToWall);
                testY = (playerY + eyeY * distanceToWall);

                if (testX < 0 || testX > MAP_WIDTH || testY < 0 || testY > MAP_HEIGHT) {
                    hitWall = true;
                    distanceToWall = depth;
                }
                else {
                    if (map[(int)testY * MAP_WIDTH + (int)testX] == '#') {
                        hitWall = true;
                    }
                }
            }

            int celling = (float)(SCREEN_HEIGHT / 2.0) - (float)SCREEN_HEIGHT / (distanceToWall * cos(rayAngle - playerA));
            int floor = SCREEN_HEIGHT - celling;

            float offsetX = testX - int(testX + .5);
            float offsetY = testY - int(testY + .5);

            float offset;

            if (abs(offsetX) > abs(offsetY))
                offset = offsetX;
            else
                offset = offsetY;

            if (offset < 0) offset += 1;

            int height = floor - celling;

            if (i == 0) cout << distanceToWall << " " << celling << " " << floor << " " << height << endl;

            RectangleShape line(Vector2f(graphicQuality, height));
            line.setPosition(Vector2f(i, celling));

            int opacity = (distanceToWall * 255 / depth) * 1.2;
            line.setFillColor(Color(0, 0, 0, opacity));

            Sprite temp = wallSprite;

            temp.setScale(height / textureWidth, height / textureHeight);

            int textureScale = (temp.getScale().x > 1) ? temp.getScale().x : 1;

            temp.setTextureRect(IntRect(offset * textureWidth, 0, textureHeight * textureScale / height * graphicQuality * 2, textureHeight));
            temp.setPosition(Vector2f(i, celling));

            window.draw(temp);
            window.draw(line);
        }

        cout << "FPS = " << 1.0f / elaspedTime << endl;

        window.display();
    }

    return 0;
}
