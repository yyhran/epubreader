#include "epubView.h"

EpubView::EpubView(QWidget* parent)
    : QTextBrowser(parent)
    , _moduNow(1)
{
    this->setMinimumWidth(400);
    this->setMinimumHeight(400);
    this->setContentsMargins(0, 0, 0, 0);

    this->openEpub(QUrl("file:d:/GitHub/epubreader/res/Mastering.epub"));
}

EpubView::~EpubView()
{

}

auto EpubView::weneedTxt() -> void { }

auto EpubView::playFile(const QUrl localRemote) -> void
{
    qDebug() << "open: " << localRemote;
    this->openEpub(localRemote);
}

auto EpubView::openEpub(const QUrl urie) -> void
{
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
}

auto EpubView::jumpEpub(const QUrl urie) -> void
{
}

auto EpubView::startHtmlGrab(bool ok) -> void
{
    if(ok)
    {

    }
}
auto EpubView::handleHtml(QString sHtml) -> void
{
}