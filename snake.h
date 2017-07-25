#ifndef SNAKE_H
#define SNAKE_H
#define SIZE 4
#include<QPointF>
#include<vector>
class Snake
{
public:
    Snake();
    void add(float x, float y);
    void removeAftern(int n);
    void remove(int n);
    void shift();
private:
    std::vector<QPointF> snake;
};

#endif // SNAKE_H
