#include "epubView.h"

EpubView::EpubView(QWidget* parent)
    : QWidget(parent)
{
    this->loadFile(QString("file:d:/GitHub/epubreader/res/Mastering.epub"));
}

EpubView::~EpubView()
{

}

auto EpubView::loadFile(const QString& path) -> void
{
    /*
    this->_playEpub = new EPUB::Converter();
    const QString ebookFile = urie.toLocalFile();
    qDebug() << "ebook file: " << ebookFile;
    this->_activeEpub = this->_playEpub->convert(ebookFile, _ZIPCACHEBOOK_);
    qDebug() << "file size: " << this->_activeEpub.size();
    if(this->_activeEpub.size() > 0)
    {
        EpubToc fox = this->_activeEpub.at(0);
        QFileInfo fileInfo(fox.jumpUrl);
        this->jumpEpub(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        qDebug() << "file url: " << fox.jumpUrl;
        emit this->paintMenuEpub(true);
    }
    */
}

auto EpubView::scroll(int amount) -> void
{
    int offset = this->_offset + amount;
    
    this->update();
}
auto EpubView::scrollPage(int amount) -> void
{
    int currentPage = this->_offset / 1;
    currentPage += amount; 
    this->update();
}
void EpubView::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
}
void EpubView::keyPressEvent(QKeyEvent* e) 
{

}
void EpubView::resizeEvent(QResizeEvent* e)
{

}