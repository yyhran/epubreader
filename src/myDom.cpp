#include "myDom.h"


QmyDom::QmyDom(bool heading)
    :decode("utf-8") // save to utf-8
{
    header = QDomDocument::createProcessingInstruction("xml",
                QString("version=\"%1\" encoding=\"%1\"").arg(decode));
    if(heading) this->appendChild(header);
}

auto QmyDom::CleanTagAsText(QString body) -> QString
{
    QTextDocument* xhtmldoc = new QTextDocument();
    xhtmldoc->setHtml(body);
    QString wbody = xhtmldoc->toPlainText();
    wbody = wbody.replace("\n", " ");
    wbody = wbody.replace("\r", "");
    wbody = wbody.replace("\t", " ");
    return wbody.trimmed();
}

auto QmyDom::CleanTagAsHTML(QString body) -> QString
{
    QTextDocument* xhtmldoc = new QTextDocument();
    xhtmldoc->setHtml(body);
    return xhtmldoc->toHtml("utf_8");
}

auto QmyDom::SetSelf(const QString fullFileName) -> bool
{
    QString errorStr;
    int errorColumn{0};
    QFile f(fullFileName);
    if(not f.exists(fullFileName)) return false;
    if(not this->setContent(&f, false, &errorStr, &errorColumn)) return false;
    else return true;
}
auto QmyDom::saveAsHtmlClean(const QString fullFileName) -> bool
{
    this->xmlUnlink(fullFileName);
    const QString xmlxx = QmyDom::toString();
    QString xcc = this->CleanTagAsHTML(xmlxx);

    QTextCodec* setCurrentCodec;
    if(this->decode == "utf-8") setCurrentCodec = QTextCodec::codecForMib(106); // utf-8
    else setCurrentCodec = QTextCodec::codecForMib(4); // latin iso

    QFile f(fullFileName);
    if(f.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream sw(&f);
        sw.setCodec(setCurrentCodec);
        sw << xcc;
        f.close();
        if(this->isFile(fullFileName)) return true;
    }
    return false;
}

auto QmyDom::SetStream(const QString stream) -> bool
{
    QString errorStr;
    int errorLine{0};
    int errorColumn{0};
    if(not this->setContent(stream, false, &errorStr, &errorLine, &errorColumn)) return false;
    else return true;
}

auto QmyDom::insertFragmentorFile(QString fragment) -> QDomElement // insert xml file or tag fragment
{
    bool check{false};
    if(this->isFile(fragment))
    {
        QFile extFile(fragment);
        extFile.open(QIODevice::ReadOnly);
        QDomDocument domExternal;
        check = domExternal.setContent(&extFile);
        if(check)
        {
            QDomElement rootExtern = domExternal.documentElement();
            QDomNode newNode = rootExtern.firstChild();
            QDomNode treallsub = domExternal.importNode(newNode, true);
            extFile.close();
            return treallsub.toElement();
        }
        else
        {
            extFile.close();
            return this->ErrorDom();
        }
    }
    else
    {
        QTextCodec* setCurrentCodec;
        if(this->decode == "utf-8") setCurrentCodec = QTextCodec::codecForMib(106); // utf8
        else setCurrentCodec = QTextCodec::codecForMib(4); // latin iso

        if(this->streamOnFile.size() > 0)
        {
            QFile f(this->streamOnFile);
            if(f.open(QFile::WriteOnly | QFile::Text))
            {
                QTextStream sw(&f);
                sw.setCodec(setCurrentCodec);
                sw << QString("<?xml version=\"1.0\"?>\n<dummyroot>%1</dummyroot>").arg(fragment);
                f.close();
                if(this->isFile(this->streamOnFile))
                {
                    return this->insertFragmentorFile(this->streamOnFile);
                }
            }
        }
        return this->ErrorDom();
    }
}

auto QmyDom::insertFragment(QDomElement e, QString fragment) -> void
{
    bool check{false};
    QDomDocument domExternal;
    check = domExternal.setContent(QString("<?xml version=\"1.0\"?>\n<dummyroot>%1</dummyroot>").arg(fragment));
    if(check)
    {
        QDomElement rootExtern = domExternal.documentElement();
        QDomNode child = rootExtern.firstChild();
        while(not child.isNull())
        {
            if(child.isElement())
            {
                e.appendChild(this->importNode(child, true).toElement());
            }
            child = child.nextSibling();
        }
    }
    else qDebug() << "### no fragment " << e.tagName(); 
}

auto QmyDom::insertFullFragment(QDomElement e, QString fragment) -> void
{
    bool check{false};
    QDomDocument domExternal;
    check = domExternal.setContent(fragment);
    if(check)
    {
        QDomElement rootExtern = domExternal.documentElement();
        QDomNode child = rootExtern.firstChild();
        while(not child.isNull())
        {
            if(child.isElement())
            {
                e.appendChild(this->importNode(child, true).toElement());
            }
            child = child.nextSibling();
        }
    }
    else qDebug() << "### no fragment " << e.tagName(); 
}
auto QmyDom::insertElemetFrag(QDomElement e, const QDomNodeList ex, QString newname) -> void
{
    const int sizex = ex.size();
    bool check = (sizex > 0 ) ? true : false;
    if(check)
    {
        for(int i{0}; i < ex.count(); ++i)
        {
            QDomNode node = ex.at(i);
            if(node.isElement())
            {
                QDomElement elment = this->importNode(node, true).toElement();
                elment.setTagName(newname);
                e.appendChild(elment);
            }
        }
    }
    else qDebug() << "### no fragment " << e.tagName();
}
auto QmyDom::StringToXML(QString t) -> QString // xml escape chars
{
    QString text = t;
    text.replace("&", "&amp;");
    text.replace("\"", "&quot;");
    text.replace("'", "&apos;");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    text.replace("\n", "&#10;");
    text.replace("\r", "&#13;");
    return text;
}

auto QmyDom::SetStreamFile(const QString fullFileName) -> void // set a work file
{
    this->streamOnFile = fullFileName;
}

auto QmyDom::saveXML(const QString fullFileName) -> bool // save to external file
{
    this->xmlUnlink(fullFileName);

    QTextCodec* setCurrentCodec;
    if(this->decode == "utf-8") setCurrentCodec = QTextCodec::codecForMib(106); // utf-8
    else setCurrentCodec = QTextCodec::codecForMib(4); // latin iso

    QFile f(fullFileName);
    if(f.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream sw(&f);
        sw.setCodec(setCurrentCodec);
        sw << QmyDom::toString();
        f.close();
        if(this->isFile(fullFileName)) return true;
    }
    return false;
}

auto QmyDom::saveXML() -> bool // save to work file if exist
{
    if(this->isFile(this->streamOnFile)) return this->saveXML(streamOnFile);
    return false;
}

auto QmyDom::Print() -> void // print to console
{
    QTextStream out(stdout);
    out << QmyDom::toString();
}

auto QmyDom::GetAtt(QDomElement e, QString name) -> QString // get attribute value from element
{
    QString textValue{""};
    QString errorValue{""};
    QDomAttr a1 = e.attributeNode(name);
    textValue = a1.value();
    if(textValue.size() > 0) return textValue;
    return errorValue;
}

auto QmyDom::root() -> QDomElement
{
    QDomElement root = this->documentElement();
    return root;
}

auto QmyDom::FilterAttribute(QDomElement element, QString attribute) -> QString
{
    QString base{"error"};
    QDomNamedNodeMap attList = element.attributes();
    int count{attList.count()};
    if(count > 0)
    {
        for(int i{0}; i < count; ++i)
        {
            QDomNode node = attList.item(i);
            if(node.nodeName() == attribute)
            {
                base = QString(node.nodeValue());
                return base;
            }
        }
    }
    return base;
}

auto QmyDom::insertTagValue(const QString name, QString value)
{
    QDomElement tag = QDomDocument::createElement(name);
    tag.appendChild(QmyDom::createTextNode(value));
    return tag;
}

auto QmyDom::saveStream(const QString fullFileName, QString xml) -> bool
{
    QTextCodec* setCurrentCodec;
    if(this->decode == "utf-8") setCurrentCodec = QTextCodec::codecForMib(106); // utf-8
    else setCurrentCodec = QTextCodec::codecForMib(4); // latin iso

    QFile f(fullFileName);
    if(f.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream sw(&f);
        sw.setCodec(setCurrentCodec);
        sw << xml;
        f.close();
        if(this->isFile(fullFileName)) return true;
    }
    return false;
}

auto QmyDom::isFile(QString fullFileName) -> bool
{
    QFile f(fullFileName);
    return f.exists();
}

auto QmyDom::xmlUnlink(QString fullFileName) -> bool
{
    QFile f(fullFileName);
    if(this->isFile(fullFileName))
    {
        if(f.remove()) return true;
    }
    return false;
}

auto QmyDom::ErrorDom() -> QDomElement // on error return </error> tag
{
    QDomElement errorElement = QDomDocument::createElement("error");
    return errorElement;
}
