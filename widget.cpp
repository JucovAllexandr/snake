#include "widget.h"
#include <iostream>
#include <QKeyEvent>
#include <QPainter>
#include <QImage>
#include <QGLWidget>
#include <math.h>
using namespace std;
Widget::Widget(QOpenGLWidget *parent)
{
    w = this->width();
    h = this->height();
    lastKey = key = 4;
    step = 2. / SZ_GRID;
    addTail = false;
    turnHead = false;
    float tx,ty = 1;
    for(int i = 0;i < SZ_GRID;i++)
    {
        tx = -1;
        for(int j = 0;j < SZ_GRID;j++)
        {
            //cout<<tx<<","<<ty<<" ";
            grid[i][j].setX(tx);
            grid[i][j].setY(ty);
            tx += step;
        }
        //cout<<endl;
        ty -= step;
    }

    QPointF t;
    int stROW = SZ_GRID/2;
    int stCOL = stROW;
    for(int i = 0; i < ST_SNK_SIZE; i++)
    {
        t.setX(grid[stROW][stCOL].x());
        t.setY(grid[stROW][stCOL++].y());
        snake.push_back(t);
    }

    tm = new QTimer();
    tm->setInterval(ST_SNAKE_SPEED);
    connect(tm,SIGNAL(timeout()),this,SLOT(updateTimer()));
    tm->start();

    eatTime = new QTimer();
    eatTime->setInterval(5000);
    connect(eatTime,SIGNAL(timeout()),this,SLOT(eatTimer()));
    eatTime->start();
    moveEat();

    mainLoop = new QTimer();
    mainLoop->setInterval(1000/60);
    connect(mainLoop,SIGNAL(timeout()),this,SLOT(update()));
    mainLoop->start();
}

Widget::~Widget()
{
    delete tm;
    delete eatTime;
}

bool Widget::isEat()
{
    return food == snake.at(0);
}

bool Widget::isCollide()
{
    for(uint i = 1;i<snake.size();i++)
        if(snake.at(0) == snake.at(i))
        {
            tailCollide = i;
            return 1;
        }
    return 0;
}

void Widget::drawGrid()
{
    glColor3f(1.,0,0);
    glPointSize(2);
    for(int i = 0;i < SZ_GRID;i++)
        for(int j = 0;j < SZ_GRID;j++)
        {
            glBegin(GL_POINTS);
            glVertex2f(grid[i][j].x(), grid[i][j].y());
            glEnd();
        }
}

void Widget::drawSnake()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);



    for(uint i = 0;i < snake.size();i++)
    {
        if(!i)
        {
            if(turnHead){
                glBindTexture(GL_TEXTURE_2D, textureID[4]);

                switch(key){
                case 6:
                    if(snake[i].y() > snake[i+1].y() && snake[i].x() == snake[i+1].x())
                        turnTop();
                    else if(snake[i].y() < snake[i+1].y() && snake[i].x() == snake[i+1].x())
                    {
                        turnBottom();
                        mirrorTexture();
                    }
                    break;
                case 4:
                    if(snake[i].y() > snake[i+1].y() && snake[i].x() == snake[i+1].x())
                    {
                        turnTop();
                        mirrorTexture();
                    }
                    else if(snake[i].y() < snake[i+1].y() && snake[i].x() == snake[i+1].x())
                    {
                        turnBottom();
                    }
                case 8:
                    if(snake[i].y() == snake[i+1].y() && snake[i].x() > snake[i+1].x())
                    {
                        turnLeft();
                        mirrorTexture();
                    }
                    else if(snake[i].y() == snake[i+1].y() && snake[i].x() < snake[i+1].x())
                    {
                        turnLeft();
                    }
                    break;
                case 2:
                    if(snake[i].y() == snake[i+1].y() && snake[i].x() > snake[i+1].x())
                    {
                        turnRight();

                    }
                    else if(snake[i].y() == snake[i+1].y() && snake[i].x() < snake[i+1].x())
                    {
                        turnRight();
                        mirrorTexture();
                    }
                    break;
                }
            }
            else{
                turnTexture();
                glBindTexture(GL_TEXTURE_2D, textureID[0]);
            }
        }
        else if(i == snake.size() -1){
            glBindTexture(GL_TEXTURE_2D,textureID[3]);
            if(snake[i].x() < snake[i-1].x() && snake[i].y() == snake[i-1].y())
                turnRight();
            else if(snake[i].x() > snake[i-1].x() && snake[i].y() == snake[i-1].y())
                turnLeft();
            else if(snake[i].x() == snake[i-1].x() && snake[i].y() > snake[i-1].y())
                turnBottom();
            else
                turnTop();
        }
        else{
            if(snake[i].x() == snake[i-1].x() && snake[i].y() != snake[i-1].y()){
                glBindTexture(GL_TEXTURE_2D, textureID[1]);
                turnTop();
            }
            else if(snake[i].x() != snake[i-1].x() && snake[i].y() == snake[i-1].y())
            {
                glBindTexture(GL_TEXTURE_2D, textureID[1]);
                turnRight();
            }

            if(i+1<snake.size())
            {
                if( (snake[i].y() == snake[i-1].y() || snake[i].y() > snake[i-1].y())
                        && (snake[i].x() <  snake[i-1].x() || snake[i].x() ==  snake[i-1].x())
                        && (snake[i].x() == snake[i+1].x() || snake[i].x() < snake[i+1].x()) //BR
                        && (snake[i].y() >  snake[i+1].y() || snake[i].y() ==  snake[i+1].y()))
                {
                    glBindTexture(GL_TEXTURE_2D, textureID[2]);
                    turnRight();
                }
                else
                    if((snake[i].y() == snake[i-1].y() || snake[i].y() < snake[i-1].y())
                            && (snake[i].x() <  snake[i-1].x() || snake[i].x() ==  snake[i-1].x())
                            && (snake[i].x() == snake[i+1].x() || snake[i].x() < snake[i+1].x()) //TR
                            && (snake[i].y() <  snake[i+1].y() || snake[i].y() ==  snake[i+1].y()))
                    {
                        glBindTexture(GL_TEXTURE_2D, textureID[2]);
                        turnTop();
                    }
                    else
                        if((snake[i].y() == snake[i-1].y() || snake[i].y() > snake[i-1].y())
                                && (snake[i].x() >  snake[i-1].x() || snake[i].x() ==  snake[i-1].x())
                                && (snake[i].x() == snake[i+1].x() || snake[i].x() > snake[i+1].x()) //BL
                                && (snake[i].y() >  snake[i+1].y() || snake[i].y() ==  snake[i+1].y()))
                        {
                            glBindTexture(GL_TEXTURE_2D, textureID[2]);
                            turnBottom();
                        }
                        else
                            if((snake[i].y() == snake[i-1].y() || snake[i].y() < snake[i-1].y())
                                    && (snake[i].x() >  snake[i-1].x() || snake[i].x() == snake[i-1].x())
                                    && (snake[i].x() == snake[i+1].x() || snake[i].x() > snake[i+1].x()) //BR
                                    && (snake[i].y() <  snake[i+1].y() || snake[i].y() ==  snake[i+1].y()))
                            {
                                glBindTexture(GL_TEXTURE_2D, textureID[2]);
                                turnLeft();
                            }
            }
        }
        glBegin(GL_QUADS);
        //cout<<snake[i].x()<<", "<<snake[i].y()<<endl;
        glTexCoord2f(LT.x(), LT.y()); glVertex2f(snake[i].x(), snake[i].y());
        glTexCoord2f(RT.x(), RT.y()); glVertex2f(snake[i].x() + step, snake[i].y());
        glTexCoord2f(RB.x(), RB.y()); glVertex2f(snake[i].x() + step, snake[i].y() - step);
        glTexCoord2f(LB.x(), LB.y()); glVertex2f(snake[i].x(), snake[i].y() - step);
        glEnd();

    }
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

void Widget::drawEat()
{
    float rad = (2./SZ_GRID)/2;
    glColor3f(1.0f,0.0f,0.0f);
    glTranslatef(food.x()+rad,food.y()-rad,0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for(int i = 0; i <= 50; i++ ) {
        float a = (float)i / 50.0f * 3.1415f * 2.0f;
        glVertex2f( cos( a ) * rad, sin( a ) * rad );
    }
    glEnd();
}

void Widget::drawPoints()
{
    QPainter p(this);
    p.setPen(Qt::black);
    p.setFont(QFont("Helvetica", 16));
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    p.drawText(0,0,w,h,0,QString::number(snake.size()));
    p.end();
}

void Widget::move()
{
    QPointF tmp, tmp2;
    QPointF add = snake.at(snake.size()-1);

    for(uint i = 0;i < snake.size();i++)
    {
        if(!i)
        {
            tmp = snake[i];
            switch (key) {
            case 8:
                snake[i].setY(tmp.y()+(2./SZ_GRID));
                break;
            case 2:
                snake[i].setY(tmp.y()-(2./SZ_GRID));
                break;
            case 6:
                snake[i].setX(tmp.x()+(2./SZ_GRID));
                break;
            case 4:
                snake[i].setX(tmp.x()-(2./SZ_GRID));
                break;
            }
        }else
        {
            tmp2 = snake[i];
            snake[i] = tmp;
            tmp = tmp2;
        }
    }
    if(addTail){
        //add.setX(add.x() - 2./SZ_GRID);
        //add.setY(add.y() - 2./SZ_GRID);
        //   cout<<tmp.x()<<", "<<tmp.y()<<" | "<<snake.at(snake.size()-1).x()<<", "<<snake.at(snake.size()-1).y()<<endl;
        snake.push_back(add);
        addTail = false;
    }
}

void Widget::moveEat()
{
    srand (time(NULL));
    int x = rand() % SZ_GRID;
    int y = rand() % SZ_GRID;
    food = grid[x][y];
    //food = grid[7][7];
}

void Widget::deleteEat()
{
    food.setX(-2);
    food.setY(-2);
}

void Widget::mirrorTexture()
{
    QPointF tmp;
    tmp = LB;
    LB = RB;
    RB = tmp;

    tmp = LT;
    LT = RT;
    RT = tmp;
}

void Widget::turnTexture()
{
    if(key == 8)
    {
        turnTop();
    }
    else if(key == 4)
    {
        turnLeft();
    }
    else if(key == 2)
    {
        turnBottom();
    }else
    {
        turnRight();
    }
}

void Widget::turnRight()
{
    LB = QPointF(0,0);
    RB = QPointF(1,0);
    RT = QPointF(1,1);
    LT = QPointF(0,1);
}

void Widget::turnLeft()
{
    LB = QPointF(1,1);
    RB = QPointF(0,1);
    RT = QPointF(0,0);
    LT = QPointF(1,0);
}

void Widget::turnTop()
{
    LB = QPointF(0,1);
    RB = QPointF(0,0);
    RT = QPointF(1,0);
    LT = QPointF(1,1);
}

void Widget::turnBottom()
{
    LB = QPointF(1,0);
    RB = QPointF(1,1);
    RT = QPointF(0,1);
    LT = QPointF(0,0);
}

void Widget::genTexture()
{
    QImage tex1, tex2, tex3, tex4, tex5;
    tex1.load(":/texture/snake/red/redHeadStaight.png");
    tex1 = QGLWidget::convertToGLFormat(tex1);
    glGenTextures(2,textureID);
    glBindTexture(GL_TEXTURE_2D,textureID[0]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tex1.width(),tex1.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex1.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    tex2.load(":/texture/snake/red/redBodyStraight.png");
    tex2 = QGLWidget::convertToGLFormat(tex2);
    glBindTexture(GL_TEXTURE_2D,textureID[1]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tex2.width(),tex2.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex2.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    tex3.load(":/texture/snake/red/redBodyTurn.png");
    tex3 = QGLWidget::convertToGLFormat(tex3);
    glBindTexture(GL_TEXTURE_2D,textureID[2]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tex3.width(),tex3.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex3.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    tex4.load(":/texture/snake/red/redTail.png");
    tex4 = QGLWidget::convertToGLFormat(tex4);
    glBindTexture(GL_TEXTURE_2D,textureID[3]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tex4.width(),tex4.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex4.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    tex5.load(":/texture/snake/red/redHeadTurn.png");
    tex5 = QGLWidget::convertToGLFormat(tex5);
    glBindTexture(GL_TEXTURE_2D,textureID[4]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tex5.width(),tex5.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex5.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


void Widget::initializeGL()
{

    genTexture();
    glMatrixMode(GL_PROJECTION);
    glClearColor(0,1.,0,0);
    glLoadIdentity();
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glOrtho(-w/(float)h,w/(float)h,-1.,1.,-1.,1.);
    drawGrid();
    drawSnake();
    drawEat();
    drawPoints();
    glFlush();
}

void Widget::resizeGL(int w, int h)
{
    this->w = w;
    this->h = h;
    glMatrixMode(GL_MODELVIEW);
    glOrtho(-2,2,-2.,2.,-2.,2.);
}

void Widget::keyPressEvent(QKeyEvent *key)
{
    if(isMove){

        if((key->key() == Qt::Key_Up || key->key() == Qt::Key_W) && this->key != 2)
            this->key = 8;
        else if((key->key() == Qt::Key_Down || key->key() == Qt::Key_S) && this->key != 8)
            this->key = 2;
        else if((key->key() == Qt::Key_Right || key->key() == Qt::Key_D)  && this->key != 4)
            this->key = 6;
        else if((key->key() == Qt::Key_Left || key->key() == Qt::Key_A)  && this->key != 6)
            this->key = 4;
        isMove = 0;
        if(lastKey!=this->key)
        turnHead = 1;
        lastKey = this->key;
    }
}

void Widget::updateTimer()
{
    turnHead = false;
    move();
    if(isEat()){
        addTail = true;
        deleteEat();
        if(tm->interval()>100)
            tm->setInterval(tm->interval()-20);
    }

    if(isCollide())
    {
        int interval = tm->interval();
        int n = snake.size() - tailCollide;
        for(int i=0;i<n;i++){
            snake.pop_back();
            interval+=20;
        }
        tm->setInterval(interval);
    }
    isMove = 1;
}

void Widget::eatTimer()
{
    moveEat();
}

