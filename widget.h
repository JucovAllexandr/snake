#ifndef WIDGET_H
#define WIDGET_H

#define SZ_GRID 16
#define ST_SNK_SIZE 4
#define ST_SNAKE_SPEED 350
#include <QWidget>
#include <vector>
#include <QOpenGLWidget>
#include <QTimer>
#include <QTime>

class Widget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit Widget(QOpenGLWidget *parent = 0);
    ~Widget();

private:
  GLuint textureID[7];
  int w,h;
  int frameCount;
  float fps;
  float step;
  int key,lastKey;
  QTimer *tm;
  bool isMove;
  bool addTail;
  bool turnHead;
  int tailCollide;
  std::vector<QPointF> snake;
  QPointF grid[SZ_GRID][SZ_GRID];
  QPointF food;
  QPointF LB, RB, LT, RT;
  QTimer *eatTime;
  QTimer *mainLoop;
  QTime frameTime;
  bool isEat();
  bool isCollide();
  void drawGrid();
  void drawSnake();
  void drawEat();
  void drawPoints();
  void move();
  void moveEat();
  void deleteEat();

  void mirrorTexture();
  void turnTexture();
  void turnRight();
  void turnLeft();
  void turnTop();
  void turnBottom();
  void genTexture();
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);
  void keyPressEvent(QKeyEvent *key);
private slots:
  void updateTimer();
  void eatTimer();
};

#endif // WIDGET_H
