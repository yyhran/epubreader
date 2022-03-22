#ifndef __MY_DOM__H
#define __MY_DOM__H

#include <QDomDocument>
#include <QtDebug>
#include <QSettings>
#include <QDomElement>
#include <QDomImplementation>
#include <QDomProcessingInstruction>
#include <QFile>
#include <QTextCodec>
#include <QString>
#include <QTextDocument>

#define POINT_TO_CM(cm) ((cm) / 28.3465058)
#define POINT_TO_MM(mm) ((mm) / 2.83465058) ////////  0.352777778
#define POINT_TO_DM(dm) ((dm) / 283.465058)
#define POINT_TO_INCH(inch) ((inch) / 72.0)
#define POINT_TO_PI(pi) ((pi) / 12)
#define POINT_TO_DD(dd) ((dd) / 154.08124)
#define POINT_TO_CC(cc) ((cc) / 12.840103)

#define MM_TO_POINT(mm) ((mm)*2.83465058)
#define CM_TO_POINT(cm) ((cm)*28.3465058) ///// 28.346456693
#define DM_TO_POINT(dm) ((dm)*283.465058)
#define INCH_TO_POINT(inch) ((inch)*72.0)
#define PI_TO_POINT(pi) ((pi)*12)
#define DD_TO_POINT(dd) ((dd)*154.08124)
#define CC_TO_POINT(cc) ((cc)*12.840103)


static inline auto genkeyurl(const QString name) -> int
{
    int base{0};
    int xsize{name.size()};
    if(15 < xsize) xsize = 15;
    for(int i{0}; i < xsize; ++i)
    {
        const QChar vox(name.at(i));
        const int unico = static_cast<int>(vox.unicode());
        base += unico;
    }
    base += 2017;
    return base;
}

class QmyDom : public QDomDocument
{
public:
    QmyDom(bool heading = true);
    ~QmyDom() { }

    auto cleanTagAsText(QString body) -> QString;
    auto cleanTagAsHTML(QString body) -> QString;
    auto getDecode() -> QString { return this->_decode; }
    auto setSelf(const QString fullFileName) -> bool;
    auto saveAsHtmlClean(const QString fullFileName) -> bool;
    auto setStream(const QString stream) -> bool;
    auto insertFragmentorFile(QString fragment) -> QDomElement; // insert xml file or tag fragment
    auto insertFragment(QDomElement e, QString fragment) -> void;
    auto insertFullFragment(QDomElement e, QString fragment) -> void;
    auto insertElemetFrag(QDomElement e, const QDomNodeList ex, QString newname) -> void;
    auto stringToXML(QString t) -> QString; // xml escape chars
    auto setStreamFile(const QString fullFileName) -> void; // set a work file
    auto saveXML(const QString fullFileName) -> bool; // save to external file
    auto saveXML() -> bool; // save to work file if exist
    auto print() -> void; // print to console
    auto getAtt(QDomElement e, QString name) -> QString; // get attribute value from element
    auto root() -> QDomElement;
    auto filterAttribute(QDomElement element, QString attribute) -> QString;
    auto insertTagValue(const QString name, QString value);
    auto saveStream(const QString fullFileName, QString xml) -> bool;
    auto isFile(QString fullFileName) -> bool;
    auto xmlUnlink(QString fullFileName) -> bool;
    auto errorDom() -> QDomElement; // on error return </error> tag

private:
    QString _decode;
    QDomProcessingInstruction _header;
    QString _streamOnFile;
};

Q_DECLARE_METATYPE(QmyDom);

#endif // __MY_DOM__H
