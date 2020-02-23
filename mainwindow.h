#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QToolBar>
#include <QFileDialog>
#include <QStringList>
#include <Qt>
#include <QMessageBox>
#include <QRegExp>
#include <QGraphicsPixmapItem>
#include <QDebug>

class MainWindow :public QMainWindow  {
    Q_OBJECT

public:
    explicit MainWindow (QWidget *parent = nullptr);

    ~MainWindow() ;
private slots:
    void openImage();
    void zoomIn();
    void zoomOut();
    void rotateClockwise();
    void rotateCounterlockwise();
    void resetTransform();
    void saveAs();
    void prevImage();
    void nextImage();
private:
    void initUI();
    void createActions();
    void setupShortcuts();
    void showImage(QString Path);


private:
    QMenu *fileMenu;
    QMenu *viewMenu;

    QToolBar *fileToolBar;
    QToolBar *viewToolBar;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    QGraphicsPixmapItem *currentImage;
    QString currentImagePath;

    QAction *openAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *rotClockAction;
    QAction *rotCountrClocAction;
    QAction *nextAction;
    QAction *prevAction;
    QAction *originalTransformAction;
};

#endif // MAINWINDOW_H
