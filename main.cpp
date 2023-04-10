//#pragma GCC optimize("O3")
#include "config.h"
#include "header.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), L"Mandelbrot", sf::Style::Default);

    sf::Image image = {};
    image.create(WINDOW_WIDTH, WINDOW_HEIGHT, sf::Color::Cyan);

    float RED  = START_KRED;
    float BLUE = START_KBLUE;

    FPS curfps = {};

    int drawFlag = 1;

    float   curX   = START_X;
    float   curY   = START_Y;
    float curDelta =  DELTA ;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        buildMandelbrot(&image, &RED, &BLUE, drawFlag, curX, curY, curDelta);

        updateFPS(&curfps);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            drawFlag ^= 1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) curY -= 3 * curDelta;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) curX -= 3 * curDelta;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) curY += 3 * curDelta;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) curX += 3 * curDelta;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) curDelta /= 1.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) curDelta *= 1.1;
        fprintf(logs, "%lg %lg\n", curX, curY);


        window.setTitle(curfps.fps);

        if (drawFlag) drawMandelbrot(&window, image);
 
    }

    dctorFPS(&curfps);

    return 0;
}

int buildMandelbrot(sf::Image *image, float *red_k, float *blue_k, bool drawFlag, const float startX, const float startY, const float DELTA) {
    const float maxY = startY + WINDOW_HEIGHT * DELTA;
    const float maxX = startX +  WINDOW_WIDTH * DELTA;

    *blue_k = *blue_k + rand() % 10 / 10.f;
    *red_k  = *red_k  + rand() % 10 / 10.f;
    
    __8f_t _startPoint = {   0  , DELTA, DELTA * 2, DELTA * 3, DELTA * 4, DELTA * 5, DELTA * 6, DELTA * 7 };

    __8f_t    _R   = {};
    __8f_t  _delta = {};

        _R.data = _mm256_set1_ps(  R  );
    _delta.data = _mm256_set1_ps(DELTA);

    __8f_t   Y   = {};
    __8f_t   X   = {};
    __8f_t   p1  = {};
    __8f_t   p2  = {};
    __8f_t   p3  = {};
    __8f_t  mask = {};
    __8f_t  curX = {};
    __8f_t  curY = {};
    
    __8i_t   N   = {};

    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        X.data = _mm256_set1_ps(startX);
        X.data = _mm256_add_ps(X.data, _startPoint.data);
        Y.data = _mm256_set1_ps(startY + y * DELTA);

        for (int x = 0; x < WINDOW_WIDTH; x += 8) {
            curX  =  X;
            curY  =  Y;

            N.data = _mm256_set1_epi32(MAX_N);

            for (int n = 0; n < MAX_N; n++) {
                p1.data = _mm256_mul_ps(curX.data, curX.data);                                                      // p1 = curX * curX;
                p2.data = _mm256_mul_ps(curY.data, curY.data);                                                      // p2 = curY * curY;

                p3.data = _mm256_add_ps(_mm256_mul_ps(curX.data, curY.data), _mm256_mul_ps(curX.data, curY.data));  //  p3 = 2 * curX * curY;
                
                curX.data = _mm256_add_ps(_mm256_sub_ps(p1.data, p2.data), X.data);                                 //  curX = p1 - p2 + x;
                curY.data = _mm256_add_ps(p3.data, Y.data);                                                         //  curY =    p3   + y;

                p3.data = _mm256_add_ps(p1.data, p2.data);

                __8f_t cmp = {};
                cmp.data = _mm256_cmp_ps(p3.data, _R.data, _CMP_LT_OQ);                                             

                int  flag = _mm256_movemask_ps(cmp.data);
                if (!flag) break;

                N.data = _mm256_sub_epi32(N.data, _mm256_castps_si256(cmp.data));
            }
            N.data = _mm256_sub_epi32(N.data, _mm256_set1_epi32(MAX_N));

            if (drawFlag) {

                sf::Color curPixel = {};

                for (int cur = 0; cur < 8; cur++) {
                    float n = N.idata[cur];

                    if (n == MAX_N)
                        curPixel = sf::Color::Black;
                    else
                        curPixel = sf::Color((n * *red_k), n, (n * *blue_k));

                    image->setPixel(x + cur, y, curPixel);
                }
            }

            X.data = _mm256_add_ps(X.data, _mm256_mul_ps(_delta.data, _mm256_set1_ps(8)));
        }
    }
    return 1;
}

void drawMandelbrot(sf::RenderWindow *window, sf::Image image) {
    sf::Texture texture = {};
    texture.loadFromImage(image);
    sf::Sprite sprite = {};
    sprite.setTexture(texture);
    window -> draw(sprite);
    window -> display();
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
