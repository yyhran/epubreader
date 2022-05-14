#include "epubView.h"

namespace EPUB {

EpubView::EpubView(QWidget* parent)
    : QTextBrowser(parent)
    , document_(Q_NULLPTR)
{
    this->connect(this, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(gotoToc(const QUrl&)));
}

EpubView::~EpubView()
{
    delete this->document_;
}

auto EpubView::loadFile(const QString& path) -> void
{
    Document* document = new Document(path, this);
    this->setDocument(document);
    delete this->document_;
    this->document_ = document;
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
