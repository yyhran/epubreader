#ifndef __EPUB_VIEW__H
#define __EPUB_VIEW__H

#include <QTextBrowser>

class EpubView : public QTextBrowser
{
    Q_OBJECT

public:
    EpubView(QWidget* parent = 0);
    ~EpubView();
};

#endif // __EPUB__VIEW__H