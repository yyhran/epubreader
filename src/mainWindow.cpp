#include "mainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , stackedWidget_(new QStackedWidget(this))
    , treeWidget_(new QTreeWidget(this))
    , fontSlider_(new FontSlider(20, 500, this))
    , _epubView(new EPUB::EpubView(this))
{
    this->resize(1000, 800);
    this->setWindowTitle(tr("Epub Reader"));

    this->initLayout();
    this->connect(this->treeWidget_, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                  this, SLOT(gotoFile(QTreeWidgetItem*, int)));

    this->setAcceptDrops(true);
    this->stackedWidget_->setAcceptDrops(false);
}

MainWindow::~MainWindow()
{

}

auto MainWindow::initLayout() -> void
{
    this->setMenu();
    this->setHomeWidge();
    this->setViewWidget();

    this->setCentralWidget(this->stackedWidget_);
    this->gotoStackedWidgetPage(0);
}

auto MainWindow::setMenu() -> void
{
    // set menubar
    auto menuBar = new QMenuBar(this);
    this->setMenuBar(menuBar);
    // option menu
    auto optionMenu = new QMenu(menuBar);
    menuBar->addAction(optionMenu->menuAction());
    optionMenu->setTitle(tr("Option(&O)"));
    // to home page
    auto homeAction = new QAction(this);
    homeAction->setText(tr("Home(&H)"));
    homeAction->setStatusTip(tr("go to home page"));
    this->connect(homeAction, SIGNAL(triggered()), this, SLOT(gotoStackedWidgetPage()));
    // open file
    auto openAction = new QAction(this);
    openAction->setText(tr("Open(&O)"));
    openAction->setStatusTip(tr("open an existing file"));
    this->connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    // close window
    auto closeAction = new QAction(this);
    closeAction->setText(tr("Close(&E)"));
    closeAction->setStatusTip(tr("clost the window"));
    this->connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    // set to menu
    optionMenu->addAction(homeAction);
    optionMenu->addSeparator();
    optionMenu->addAction(openAction);
    optionMenu->addSeparator();
    optionMenu->addAction(closeAction);
}

auto MainWindow::setHomeWidge() -> void
{
    QWidget* homeWidget = new QWidget(this);
    QLabel* label = new QLabel(homeWidget);
    label->setText(tr("Hello epub!!!"));
    label->setAlignment(Qt::AlignCenter);

    QHBoxLayout* mainLayout = new QHBoxLayout(homeWidget);
    mainLayout->addWidget(label);
    homeWidget->setLayout(mainLayout);
    this->stackedWidget_->addWidget(homeWidget);
}

auto MainWindow::setViewWidget() -> void
{
    QMainWindow* viewWidget = new QMainWindow(this);
    QDockWidget* tocWidget = new QDockWidget(tr("TOC"), viewWidget);
    tocWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    tocWidget->setFeatures(QDockWidget::DockWidgetMovable);
    tocWidget->setWidget(this->treeWidget_);
    viewWidget->setCentralWidget(this->_epubView);
    viewWidget->addDockWidget(Qt::RightDockWidgetArea, tocWidget);

    this->setBottomWidget(viewWidget);
    this->stackedWidget_->addWidget(viewWidget);
}

auto MainWindow::setBottomWidget(QMainWindow* viewWidget) -> void
{
    auto statusBar = new QStatusBar(this);
    this->connect(this->fontSlider_, &FontSlider::valueChanged, this, [&](int value)
    {
        int size = this->font_.pointSize() * value / 100;
        this->_epubView->getDocument()->setDefaultFont(QFont(this->font_.family(), size));
    });

    statusBar->addPermanentWidget(this->fontSlider_);
    viewWidget->setStatusBar(statusBar);
}

auto MainWindow::openFile(const QString& fileName) -> void
{
    this->fontSlider_->resizeValue();
    this->gotoStackedWidgetPage(1);
    this->_epubView->loadFile(fileName);
    this->setToc();
}

auto MainWindow::gotoStackedWidgetPage(int page) -> void
{
    Q_ASSERT(page >= 0);
    this->stackedWidget_->setCurrentIndex(page);
}

auto MainWindow::gotoFile(QTreeWidgetItem* item, int index) -> void
{
    QString file = this->tocMap_[item].src;
    int pos = file.indexOf("#");
    QString fileName = file.left(pos);
    QString filePos = pos == -1 ? "#" : file.mid(pos);
    if(fileName != this->_epubView->getDocument()->openedFile())
    {
        this->_epubView->setFile(fileName);
    }
    this->_epubView->setPos(filePos);
}

auto MainWindow::openFile() -> void
{
    QFileDialog fileGet;
    auto fileName = fileGet.getOpenFileName(this, tr("Open File"), tr("./"), tr("EPUBS(*.epub)"));
    if("" == fileName) return;

    this->openFile(fileName);
}

auto MainWindow::setToc() -> void
{
    // clear
    this->treeWidget_->clear();
    this->metaInfo_.clear();
    this->tocMap_.clear();
    this->font_.cleanup();

    this->font_ = this->_epubView->getDocument()->getFont();
    this->metaInfo_ = this->_epubView->getDocument()->getMeta();
    this->treeWidget_->headerItem()->setText(0, this->metaInfo_["title"].toStdList().front());

    auto tocData = this->_epubView->getDocument()->getToc();
    QMap<QString, QTreeWidgetItem*> tocMap;
    for(auto&& data : tocData)
    {
        auto text = data.text;
        auto upper = data.upper;
        QTreeWidgetItem* item = Q_NULLPTR;
        if("" == upper)
        {
            item = new QTreeWidgetItem(this->treeWidget_, QStringList(text));
        }
        else
        {
            item = new QTreeWidgetItem(tocMap[upper], QStringList(text));
        }
        this->tocMap_.insert(item, data);
        tocMap.insert(text, item);
    }
}

auto MainWindow::dragEnterEvent(QDragEnterEvent* event) -> void
{
    if(!event->mimeData()->urls()[0].fileName().right(4).compare("epub"))
    {
        event->acceptProposedAction();
    }
    else event->ignore();
}

auto MainWindow::dropEvent(QDropEvent* event) -> void
{
    auto qm = event->mimeData();
    auto fileName = qm->urls()[0].toLocalFile();
    this->openFile(fileName);
}
