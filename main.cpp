#pragma GCC optimize("O3")
#include "config.h"
#include "header.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), L"Mandelbrot", sf::Style::Default);

    sf::Image image = {};
    image.create(WINDOW_WIDTH, WINDOW_HEIGHT, sf::Color::Green);

    int RED  = START_KRED;
    int BLUE = START_KBLUE;

    FPS curfps = {};

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        buildMandelbrot(&image, &RED, &BLUE);

        updateFPS(&curfps);

        window.setTitle(curfps.fps);
        sf::Texture texture = {};
        texture.loadFromImage(image);
        sf::Sprite sprite = {};
        sprite.setTexture(texture);
        window.draw(sprite);
        window.display();

    }

    dctorFPS(&curfps);

    return 0;
}

int buildMandelbrot(sf::Image *image, int *red_k, int *blue_k) {
    const float maxY = START_Y + WINDOW_HEIGHT * DELTA;
    const float maxX = START_X +  WINDOW_WIDTH * DELTA;

    *blue_k = *blue_k + rand() % 2 - (rand() % 2) * 2;
    *red_k  = *red_k  + rand() % 2 - (rand() % 2) * 2;
    
    __8f_t _startPoint = {   0  , DELTA, DELTA * 2, DELTA * 3, DELTA * 4, DELTA * 5, DELTA * 6, DELTA * 7 };
    __8f_t   _delta    = { DELTA, DELTA,   DELTA  ,   DELTA  ,   DELTA  ,   DELTA  ,   DELTA  ,   DELTA   };

    for (float y = START_Y; y < maxY; y += DELTA) {
        for (float x = START_X; x < maxX; x += DELTA) {
            float curX  = x;
            float curY  = y;
             int   n    = 0;

            for (; n < MAX_N; n++) {
                float p1 =   curX * curX  ;
                float p2 =   curY * curY  ;
                float p3 = 2 * curX * curY;
                
                curX = p1 - p2 + x;
                curY =   p3    + y;

                if (p1 + p2 > R) break;
            }

            curX = (x - START_X) / DELTA;
            curY = (y - START_Y) / DELTA;

            sf::Color curPixel = {};

            if (n == MAX_N)
                curPixel = sf::Color::Black;
            else 
                curPixel = sf::Color((n * *red_k), n, (n * *blue_k));

            image -> setPixel(curX, curY, curPixel);
        }
    }
    return 1;
}

void updateFPS(FPS* fps) {
    if (fps->clock.getElapsedTime().asSeconds() >= 1.f) {
        getFPS(fps);

        fps->frame = 0;
        fps->clock.restart();
    }
    fps->frame++;
}

void getFPS(FPS* fps) {
    _itoa_s(fps->frame, fps -> fps, FPS_BUFFER_SIZE, 10);
}

void dctorFPS(FPS* fps) {
    free(fps->fps);
    fps->frame = 0;
}