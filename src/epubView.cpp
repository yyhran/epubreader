#include "epubView.h"

namespace EPUB {

EpubView::EpubView(QWidget* parent)
    : QTextBrowser(parent)
    , _document(Q_NULLPTR)
    , _offset(0)
{
    this->connect(this, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(gotoToc(const QUrl&)));
}

EpubView::~EpubView()
{
    delete this->_document;
}

auto EpubView::loadFile(const QString& path) -> void
{
    Document* document = new Document(path, this);
    this->setDocument(document);
    delete this->_document;
    this->_document = document;
}

auto EpubView::setPos(const QUrl& url) -> void
{
    emit this->anchorClicked(url);
}

auto EpubView::gotoToc(const QUrl& url) -> void
{
    this->setSource(url);
}

}
