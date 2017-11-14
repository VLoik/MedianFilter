#ifndef THREADFILTER_H
#define THREADFILTER_H
#include "QImage"
#include "QRunnable"
#include "QMutex"
#include "QQuickImageProvider"
#include "QThreadPool"
#include "QTime"

class FilterConnector;
class ThreadFilter :public QObject, public QRunnable
{
    Q_OBJECT
public:
    ThreadFilter(QObject *parent = 0): QObject(parent){};
    ~ThreadFilter();
    void run();
    inline void init(QImage *image, int r){img=image; rad=r;};
private:
    QImage *img;
    int rad;
signals:
    void pixelDone(int n);
    void finished();
};

class FilterConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rad READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(int numthread READ thread WRITE setNumthread NOTIFY numthreadChanged)

public:
    FilterConnector();
    ~FilterConnector();

    int radius();
    int thread();
    void setRadius(const int &r);

    QString filename();
    void setFilename(const QString &f);
    void setNumthread(const int &r);

public slots:
    void start();
    void update_p(int n);
    void finish();
    void stop();

signals:
    void radiusChanged();
    void filenameChanged();
    void numthreadChanged();
    void filteringDone(int t);
    void pixelDone(QString n);

private:
    int _rad;
    int _nth;
    QString _filename;
    QThreadPool *threadPool;
    ThreadFilter *th;
    QTime time;
    QImage *img;
    QImage *buf_img;
};

class ImageProvider : public QQuickImageProvider
{
public:
    //ImageProvider(QImage *img);
    ImageProvider(): QQuickImageProvider(QQuickImageProvider::Pixmap){}

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    QImage img_;

};
#endif // THREADFILTER_H
