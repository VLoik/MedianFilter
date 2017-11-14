#include "threadfilter.h"
#include "QDebug"
#include "QThread"
#include "QImage"
#include "QVector"
#include "QPixmap"
#include "QDebug"
#include "threadfilter.h"
#include "QThreadPool"
#include "QTime"
#include "QQmlContext"

int current=0;
QMutex mutex;
int done=0;


ThreadFilter::~ThreadFilter()
{
    qDebug()<<"delete";
}

void ThreadFilter::run()
{
    if(current==0) current = (rad+1)/2;
    int x,y,i,j;
    QVector<int> vecr;
    QVector<int> vecg;
    QVector<int> vecb;
    mutex.lock();
    y=current;
    current++;
    mutex.unlock();

    while(current <= img->height()- (rad + 1)/2)
    {
        if(current>=img->height()- (rad - 1)/2)
        {
            return;
        }
        for (x = (rad + 1)/ 2; x <= img->width()- (rad + 1)/2; x++)
        {
            for (i = x - (rad-1)/2; i <= x + (rad-1)/2; i++)
            {
                for(j = y - (rad-1)/2; j <= y + (rad-1)/2; j++)
                {
                    vecr.append(QColor(img->pixel(i,j)).red());
                    vecg.append(QColor(img->pixel(i,j)).green());
                    vecb.append(QColor(img->pixel(i,j)).blue());
                }
            }
            qSort(vecr.begin(),vecr.end());
            qSort(vecg.begin(),vecg.end());
            qSort(vecb.begin(),vecb.end());
            mutex.lock();
            img->setPixel(x,y, QColor(vecr.at((rad*rad-1)/2),vecg.at((rad*rad-1)/2),vecb.at((rad*rad-1)/2),0).rgb());
            mutex.unlock();
            vecr.clear();
            vecg.clear();
            vecb.clear();
            mutex.lock();
            done++;
            emit pixelDone(done);
            mutex.unlock();
        }
        //qDebug()<<current << " Thread: "<< QThread::currentThread();
        //qDebug()<<current;
        //qDebug()<<img->height()- (rad - 1)/2;
        mutex.lock();
        y=current;
        current++;
        if(current==img->height()- (rad - 1)/2)
        {
            qDebug()<<current;
            emit finished();
            mutex.unlock();
            return;
        }
        mutex.unlock();

    }
}

QString FilterConnector::filename()
{
    return _filename;
}

void FilterConnector::setFilename(const QString &f)
{
    if (f != _filename) {
        _filename = f;
        emit filenameChanged();
    }
}

void FilterConnector::setNumthread(const int &r)
{
    if (r != _nth) {
        _nth = r;
        emit radiusChanged();
    }
}

void FilterConnector::start()
{
    current=0;
    done=0;
    img = new QImage(_filename);
    int rad =_rad;
    int x,y,i,j,z;
    buf_img = new QImage(img->width()+(2*rad-1)/2, img->height()+(2*rad-1)/2, QImage::Format_RGB32);

    //ugol l v
    for(x=0; x<rad/2; x++)
    {
        for(y=0; y<rad/2; y++)
        {
            buf_img->setPixel(x,y,QColor(img->pixel(x,y)).rgb());
        }
    }
    //ugol r v
    for(x=img->width(); x<img->width()+rad/2; x++)
    {
        for(y=0; y<rad/2; y++)
        {
            buf_img->setPixel(x+rad/2,y,QColor(img->pixel(x-rad/2,y)).rgb());
        }
    }
    //ugol r n
    for(x=img->width(); x<img->width()+rad/2; x++)
    {
        for(y=img->height(); y<img->height()+rad/2; y++)
        {
            buf_img->setPixel(x+rad/2,y+rad/2,QColor(img->pixel(x-rad/2,y-rad/2)).rgb());
        }
    }
    //ugol l n
    for(x=0; x<rad/2; x++)
    {
        for(y=img->height(); y<img->height()+rad/2; y++)
        {
            buf_img->setPixel(x,y+rad/2,QColor(img->pixel(x,y-rad/2)).rgb());
        }
    }
    //v
    for(x=0; x<img->width(); x++)
    {
        for(y=0; y<rad/2; y++)
        {
            buf_img->setPixel(x+rad/2,y,QColor(img->pixel(x,y)).rgb());
        }
    }
    //r
    for(x=img->width(); x<img->width()+rad/2; x++)
    {
        for(y=0; y<img->height(); y++)
        {
            buf_img->setPixel(x+rad/2,y+rad/2,QColor(img->pixel(x-rad/2,y)).rgb());
        }
    }
    //n
    for(x=0; x<img->width(); x++)
    {
        for(y=img->height(); y<img->height()+rad/2; y++)
        {
            buf_img->setPixel(x+rad/2,y+rad/2,QColor(img->pixel(x,y-rad/2)).rgb());
        }
    }
    //l
    for(x=0; x<rad/2; x++)
    {
        for(y=0; y<img->height(); y++)
        {
            buf_img->setPixel(x,y+rad/2,QColor(img->pixel(x,y)).rgb());
        }
    }
    //img
    for(x=0; x<img->width(); x++)
    {
        for(y=0; y<img->height(); y++)
        {
            buf_img->setPixel(x+rad/2,y+rad/2,QColor(img->pixel(x,y)).rgb());
        }
    }

    threadPool->clear();
    time.start();
    for(int i=0; i<thread();i++)
    {
        th = new ThreadFilter();
        th->init(buf_img, rad);
        th->setAutoDelete(true);
        connect(th, SIGNAL(pixelDone(int)),this, SLOT(update_p(int)));
        connect(th, SIGNAL(finished()),this, SLOT(finish()));
        threadPool->start(th);
    }


    qDebug() << "hello from GUI thread " << QThread::currentThread();
}

void FilterConnector::update_p(int n)
{
    int size = img->width()*img->height();
    double p;
    p=(double)n/size;
    emit pixelDone(QString::number((int)(p*100)));
}

void FilterConnector::finish()
{
    threadPool->clear();
    int t = time.elapsed();

    QImage timg = buf_img->copy(_rad/2, _rad/2, img->width(), img->height());
    img=&timg;


    QQmlEngine *engine = QQmlEngine::contextForObject(this)->engine();
    ImageProvider *imgpr = new ImageProvider;
    imgpr->img_=*img;
    engine->addImageProvider("filterimg", imgpr);
    emit filteringDone(t);
}

void FilterConnector::stop()
{
    threadPool->clear();
    qDebug()<<"Stop";
}

FilterConnector::FilterConnector()
{
    threadPool = QThreadPool::globalInstance();

}

FilterConnector::~FilterConnector()
{

}

int FilterConnector::radius()
{
    return _rad;
}

int FilterConnector::thread()
{
    return _nth;
}

void FilterConnector::setRadius(const int &r)
{
    if (r != _rad) {
        _rad = r;
        emit radiusChanged();
    }
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    return QPixmap::fromImage(img_);
}
