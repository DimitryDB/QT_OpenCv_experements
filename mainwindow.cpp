#include <QDebug>
#include <QRegExp>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QPluginLoader>

#include "opencv2/opencv.hpp"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    fileMenu(nullptr),
    viewMenu(nullptr),
    currentImage(nullptr),
    pixmapBase(nullptr)

{
    initUI();
    loadPlugins();
}

MainWindow::~MainWindow() {
    if (pixmapBase)
       delete pixmapBase;
}



void MainWindow::initUI() {
    this->resize(800,600);

    //menu
    fileMenu = menuBar()->addMenu("&File");
    viewMenu = menuBar()->addMenu("&View");
    editMenu = menuBar()->addMenu("&Edit");


    //toolbar
    fileToolBar = addToolBar("File");
    viewToolBar = addToolBar("View");
    editToolBar = addToolBar("Edit");

    //main area
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    setCentralWidget(imageView);

    //status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);

    createActions();

}

void MainWindow::createActions() {
    openAction = new QAction("&Open" ,this);
    fileMenu->addAction(openAction);
    saveAsAction = new QAction("&Save as" ,this);
    fileMenu->addAction(saveAsAction);
    exitAction = new QAction("E&xit" ,this);
    fileMenu->addAction(exitAction);

    zoomInAction = new QAction("Zoom in" ,this);
    viewMenu->addAction(zoomInAction);
    zoomOutAction = new QAction("Zoom Out" ,this);
    viewMenu->addAction(zoomOutAction);
    rotClockAction = new QAction("Rotate Clockwise" ,this);
    viewMenu->addAction(rotClockAction);
    rotCountrClocAction = new QAction("Rotate CClockWise" ,this);
    viewMenu->addAction(rotCountrClocAction);
    originalTransformAction = new QAction("&Original Transform" ,this);
    viewMenu->addAction(originalTransformAction);
    prevAction = new QAction("&Previous Image" ,this);
    viewMenu->addAction(prevAction);
    nextAction = new QAction("&Next Image" ,this);
    viewMenu->addAction(nextAction);

    undoModifications = new QAction("Undo Modifications",this);
    editMenu->addAction(undoModifications);
    blurAction = new QAction("Blur",this);
    editMenu->addAction(blurAction);


    fileToolBar->addAction(openAction);
    viewToolBar->addAction(zoomInAction);
    viewToolBar->addAction(zoomOutAction);
    viewToolBar->addAction(rotClockAction);
    viewToolBar->addAction(rotCountrClocAction);
    viewToolBar->addAction(originalTransformAction);
    viewToolBar->addAction(prevAction);
    viewToolBar->addAction(nextAction);
    editToolBar->addAction(undoModifications);
    editToolBar->addAction(blurAction);



    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()),
            Qt::QueuedConnection);
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(rotClockAction, SIGNAL(triggered(bool)), this, SLOT(rotateClockwise()));
    connect(rotCountrClocAction, SIGNAL(triggered(bool)), this, SLOT(rotateCounterlockwise()));
    connect(originalTransformAction, SIGNAL(triggered(bool)), this, SLOT(resetTransform()));
    connect(saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(prevAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));
    connect(blurAction, SIGNAL(triggered(bool)), this, SLOT( blur()));
    connect(undoModifications, SIGNAL(triggered(bool)), this, SLOT( undo()));
    setupShortcuts();
}

void MainWindow::setupShortcuts() {
    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus <<Qt::Key_Equal;
    zoomInAction->setShortcuts(shortcuts);
    shortcuts.clear();
    shortcuts << Qt::Key_Minus <<Qt::Key_Underscore;
    zoomOutAction->setShortcuts(shortcuts);
    shortcuts.clear();
    shortcuts << Qt::Key_Up <<Qt::Key_Left;
    prevAction->setShortcuts(shortcuts);
    shortcuts.clear();
    shortcuts << Qt::Key_Down <<Qt::Key_Right;
    nextAction->setShortcuts(shortcuts);
}
void MainWindow::showImage(QString path) {
    currentImagePath = path;
    imageScene->clear();
    imageView->resetTransform();
    QPixmap image(path);
    if (pixmapBase)
        delete pixmapBase;
    pixmapBase = new QPixmap(image);
    currentImage = imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
            .arg(image.height()).arg(QFile(path).size());
    mainStatusLabel->setTextFormat(Qt::PlainText);
    mainStatusLabel->setText(status);
}
void MainWindow::loadPlugins() {
    QDir pluginsDir(QApplication::instance()->applicationDirPath()+"/plugins");
    QStringList nameFilters;
    nameFilters << "*.so" << "*.dll" << "*.dlib";
    QFileInfoList plugins = pluginsDir.entryInfoList(nameFilters,
                                                 QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    foreach (QFileInfo plugin, plugins) {
        QPluginLoader pluginloader(plugin.absoluteFilePath(),this);
        EditorPluginInterface *plugin_ptr = dynamic_cast<EditorPluginInterface*>(pluginloader.instance());
        if(plugin_ptr) {
            QAction *action = new QAction(plugin_ptr->name());
            editMenu->addAction(action);
            editToolBar->addAction(action);
            editPlugins[plugin_ptr->name()] = plugin_ptr;
            connect(action,SIGNAL(triggered(bool)), this, SLOT(pluginPerform()));
        } else {
            qDebug() << "bad plugin: " << plugin.absoluteFilePath();
        }
    }
}
void MainWindow::pluginPerform() {
    if(currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No image to edit");
    }
    QAction *active_action = qobject_cast<QAction*>(sender());
    EditorPluginInterface *plugin_ptr = editPlugins[active_action->text()];
    if(!plugin_ptr) {
        QMessageBox::information(this, "Information", "No plugin is found");
        return;
    }
    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat( image.height(), image.width(), CV_8UC3,
                          image.bits(),image.bytesPerLine());
    plugin_ptr->edit(mat,mat);
    QImage image_edited(mat.data, mat.cols, mat.rows,
                        mat.step, QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(image_edited);
    imageScene->clear();
    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();
    imageView->setSceneRect(image.rect());

    QString status = QString("(editted image), %1x%2").arg(pixmap.width()).arg(pixmap.height());
    mainStatusLabel->setText(status);
}

void MainWindow::zoomIn() {
    imageView->scale(1.2,1.2);
}
void MainWindow::zoomOut() {
    imageView->scale(0.8,0.8);
}
void MainWindow::rotateClockwise() {
    imageView->rotate(90);
}
void MainWindow::rotateCounterlockwise() {
    imageView->rotate(-90);
}
void MainWindow::resetTransform() {
    imageView->resetTransform();
}
void MainWindow::openImage() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images(*.png *.bmp *.jpg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }
}
void MainWindow::saveAs() {
    if (currentImage == nullptr) {
        QMessageBox::information(this,"Information","Nothing to save:");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save image as...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images(*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if (QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this,"Information","Save Error: bad format or filname.");
        }
    }
}
void MainWindow::prevImage() {
    QFileInfo current(currentImagePath);
    QDir dir = current.dir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    ;
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if (idx == 1)
       prevAction->setEnabled(false);
    if (idx >0) {
        showImage(dir.absoluteFilePath(fileNames.at(idx-1)));
    }
    //else {
    //    QMessageBox::information(this,"Information", "Current image is the first one.");
    //}
    nextAction->setEnabled(true);
    //qDebug()<< dir.absoluteFilePath(fileNames.at(idx));
}
void MainWindow::nextImage() {
    QFileInfo current(currentImagePath);
    QDir dir = current.dir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if (idx == fileNames.size()-2)
       nextAction->setEnabled(false);
    if (idx < fileNames.size()-1) {
        showImage(dir.absoluteFilePath(fileNames.at(idx+1)));
    }
    //else {
    //    QMessageBox::information(this,"Information", "Current image is the last one.");
    //}
    prevAction->setEnabled(true);
}
void MainWindow::blur() {
    if (currentImage == nullptr) {
        QMessageBox::information(this,"Information", "No image to edit.");
        return;
    }
    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat( image.height(), image.width(), CV_8UC3,
                          image.bits(),image.bytesPerLine());
    cv::Mat tmp;
    cv::blur(mat,tmp, cv::Size(8,8));
    mat = tmp;
    QImage image_blured(mat.data, mat.cols, mat.rows,
                        mat.step, QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(image_blured);
    imageScene->clear();
    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();
    imageView->setSceneRect(image.rect());

    QString status = QString("(editted image), %1x%2").arg(pixmap.width()).arg(pixmap.height());
    mainStatusLabel->setText(status);
}
void MainWindow::undo() {
    imageScene->clear();
    currentImage = imageScene->addPixmap(*pixmapBase);
    imageScene->update();
}

