#include "epubView.h"

namespace EPUB {

EpubView::EpubView(QWidget* parent)
    : QTextBrowser(parent)
    , _document(Q_NULLPTR)
    , _offset(0)
{
}

EpubView::~EpubView()
{

}

auto EpubView::loadFile(const QString& path) -> void
{
    delete this->_document;
    this->_document = new Document(path, this);
    this->setDocument(this->_document);
}

}
