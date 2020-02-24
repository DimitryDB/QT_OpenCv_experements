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
#include <QGraphicsPixmapItem>
#include <QMap>

#include "editor_plugin_interface.h"


class MainWindow :public QMainWindow  {
    Q_OBJECT

public:
    explicit MainWindow (QWidget *parent = nullptr);

    ~MainWindow() ;
private slots:
    void pluginPerform();
    void openImage();
    void zoomIn();
    void zoomOut();
    void rotateClockwise();
    void rotateCounterlockwise();
    void resetTransform();
    void saveAs();
    void prevImage();
    void nextImage();
    void blur();
    void undo();
private:
    void initUI();
    void createActions();
    void setupShortcuts();
    void showImage(QString Path);
    void loadPlugins();



private:
    QMap<QString, EditorPluginInterface*> editPlugins;
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *editMenu;

    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *editToolBar;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    QGraphicsPixmapItem *currentImage;
    QString currentImagePath;
    QPixmap *pixmapBase;

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
    QAction *undoModifications;

    QAction *blurAction;
};

#endif // MAINWINDOW_H
