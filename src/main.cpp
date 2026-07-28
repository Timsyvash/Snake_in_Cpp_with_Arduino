#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

typedef struct Snake Snake;
struct Snake
{
    int head[2];
    int body[40][2];
    int len;
    int dir[2];
};

typedef struct Apple Apple;
struct Apple
{
    int rPos;
    int cPos;
};

const short width = 8;
const short height = 8;
const short matrix_pin = 7;
const short num_leds = width * height;

Adafruit_NeoPixel matrix(num_leds, matrix_pin, NEO_GRB + NEO_KHZ800);

const int varXPin = A3;
const int varYPin = A4;

byte pic[8] = {0, 0, 0, 0, 0, 0, 0, 0};

Snake snake = {{1, 5}, {{0, 5}, {1, 5}}, 2, {1, 0}};
Apple apple = {(int)random(0, 8), (int)random(0, 8)};

float oldTime = 0;
float timer = 0;
float updateRate = 3;

int i, j;

float calculateDeltaTime()
{
    float currentTime = millis();
    float dt = currentTime - oldTime;
    oldTime = currentTime;
    return dt;
}

void reset()
{
    for (int j = 0; j < 8; j++)
    {
        pic[j] = 0;
    }
}

void removeFirst()
{
    for (j = 1; j < snake.len; j++)
    {
        snake.body[j - 1][0] = snake.body[j][0];
        snake.body[j - 1][1] = snake.body[j][1];
    }
}

void Update()
{
    reset();

    int newHead[2] = {snake.head[0] + snake.dir[0], snake.head[1] + snake.dir[1]};

    if (newHead[0] == 8)
    {
        newHead[0] = 0;
    }
    else if (newHead[0] == -1)
    {
        newHead[0] = 7;
    }
    else if (newHead[1] == 8)
    {
        newHead[1] = 0;
    }
    else if (newHead[1] == -1)
    {
        newHead[1] = 7;
    }

    for (j = 0; j < snake.len; j++)
    {
        if (snake.body[j][0] == newHead[0] && snake.body[j][1] == newHead[1])
        {
            delay(1000);
            snake = {{1, 5}, {{0, 5}, {1, 5}}, 2, {1, 0}};
            apple = {(int)random(0, 8), (int)random(0, 8)};
            return;
        }
    }

    if (newHead[0] == apple.rPos && newHead[1] == apple.cPos)
    {
        snake.len = snake.len + 1;
        apple.rPos = (int)random(0, 8);
        apple.cPos = (int)random(0, 8);
    }
    else
    {
        removeFirst();
    }

    snake.body[snake.len - 1][0] = newHead[0];
    snake.body[snake.len - 1][1] = newHead[1];

    snake.head[0] = newHead[0];
    snake.head[1] = newHead[1];

    for (j = 0; j < snake.len; j++)
    {
        pic[snake.body[j][0]] |= 128 >> snake.body[j][1];
    }
    pic[apple.rPos] |= 128 >> apple.cPos;
}

void Render()
{
    matrix.clear(); // Очищаємо матрицю перед малюванням нового кадру

    for (int i = 0; i < 8; i++) // Проходимо по рядках (y)
    {
        for (int j = 0; j < 8; j++) // Проходимо по стовпчиках (x)
        {
            // Перевіряємо, чи встановлений j-ий біт у рядку i
            if (pic[i] & (128 >> j))
            {
                // Для стандартної матриці з прямим з'єднанням (Progressive)
                int pixelIndex = i * 8 + j;

                // Встановлюємо колір (R, G, B) — наприклад, червоний
                matrix.setPixelColor(pixelIndex, matrix.Color(255, 0, 0));
            }
        }
    }

    matrix.show(); // Відправляємо сигнал на матрицю для оновлення
}

void setup()
{
    matrix.begin();
    matrix.setBrightness(5);
    matrix.clear();
    matrix.show();

    pinMode(varXPin, INPUT);
    pinMode(varYPin, INPUT);
}

void loop()
{
    float deltaTime = calculateDeltaTime();
    timer += deltaTime;

    int xVal = analogRead(varXPin);
    int yVal = analogRead(varYPin);

    if (xVal < 100 && snake.dir[1] == 0)
    {
        snake.dir[0] = 0;
        snake.dir[1] = -1;
    }
    else if (xVal > 920 && snake.dir[1] == 0)
    {
        snake.dir[0] = 0;
        snake.dir[1] = 1;
    }
    else if (yVal < 100 && snake.dir[0] == 0)
    {
        snake.dir[0] = -1;
        snake.dir[1] = 0;
    }
    else if (yVal > 920 && snake.dir[0] == 0)
    {
        snake.dir[0] = 1;
        snake.dir[1] = 0;
    }

    if (timer > 1000 / updateRate)
    {
        timer = 0;
        Update();
    }

    Render();
}
