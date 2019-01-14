#include <QtGui>
#include <QTextEdit>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QLabel>
#include <QDockWidget>
#include <QCalendarWidget>
#include "mainwin.h"
#include "editor.h"



MainWindow::MainWindow()
{
    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateActions()));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createConnections();
    QCalendarWidget *e=new QCalendarWidget;
    createDockWin(e,"Calendar",Qt::RightDockWidgetArea);

    setWindowIcon(QPixmap(":/icons/editor_icon.png"));
    setWindowTitle(tr("Simple Editor(SE)"));
    QTimer::singleShot(0, this, SLOT(loadFiles()));
}

void MainWindow::loadFiles()
{
    QStringList args = QApplication::arguments();
    args.removeFirst();
    if (!args.isEmpty()) {
        foreach (QString arg, args)
            openFile(arg);
        mdiArea->cascadeSubWindows();
    } else {
        newFile();
    }
    mdiArea->activateNextSubWindow();
}

void MainWindow::setLeftAlign()
{
    if(activeEditor())
        activeEditor()->alignLeft();
}

void MainWindow::setRightAlign()
{
    if(activeEditor())
        activeEditor()->alignRight();
}

void MainWindow::setCenterAlign()
{
    if(activeEditor())
        activeEditor()->alignCenter();
}

void MainWindow::setJustifyAlign()
{
    if(activeEditor())
        activeEditor()->alignJustify();
}

void MainWindow::setBoldStyle()
{
    if(activeEditor()){
        if(activeEditor()->fontWeight()<=CENTER_BOLD){
            activeEditor()->setBold(true);
        }
        else if(activeEditor()->fontWeight()>=CENTER_BOLD) {
            activeEditor()->setBold(false);
        }
    }
}


void MainWindow::newFile()
{
    Editor *editor = new Editor;
    editor->newFile();
    addEditor(editor);
}

void MainWindow::openFile(const QString &fileName)
{
    Editor *editor = Editor::openFile(fileName, this);
    if (editor)
        addEditor(editor);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (!mdiArea->subWindowList().isEmpty()) {
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::open()
{
    Editor *editor = Editor::open(this);
    if (editor)
        addEditor(editor);
}

void MainWindow::save()
{
    if (activeEditor())
        activeEditor()->save();
}

void MainWindow::saveAs()
{
    if (activeEditor())
        activeEditor()->saveAs();
}

void MainWindow::cut()
{
    if (activeEditor())
        activeEditor()->cut();
}

void MainWindow::copy()
{
    if (activeEditor())
        activeEditor()->copy();
}

void MainWindow::paste()
{
    if (activeEditor())
        activeEditor()->paste();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Editor"),
                       tr("<h2>Editor 0.1</h2>"
                          "<p>Copyright &copy; 2018"
                          "<p>Simple Editor is a small application that demonstrates "));
}

void MainWindow::updateActions()
{
    bool hasEditor = (activeEditor() != nullptr);
    bool hasSelection = activeEditor()
            && activeEditor()->textCursor().hasSelection();

    saveAction->setEnabled(hasEditor);
    saveAsAction->setEnabled(hasEditor);
    cutAction->setEnabled(hasSelection);
    copyAction->setEnabled(hasSelection);
    pasteAction->setEnabled(hasEditor);
    closeAction->setEnabled(hasEditor);
    closeAllAction->setEnabled(hasEditor);
    tileAction->setEnabled(hasEditor);
    cascadeAction->setEnabled(hasEditor);
    nextAction->setEnabled(hasEditor);
    previousAction->setEnabled(hasEditor);
    separatorAction->setVisible(hasEditor);

    if (activeEditor())
        activeEditor()->windowMenuAction()->setChecked(true);
}

void MainWindow::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon(":/icons/new.png"));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("&Open..."), this);
    openAction->setIcon(QIcon(":/icons/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/icons/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the file to disk"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("Save &As..."), this);
    saveAsAction->setIcon(QIcon(":/icons/save_as.png"));
    saveAsAction->setStatusTip(tr("Save the file under a new name"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    cutAction = new QAction(tr("Cu&t"), this);
    cutAction->setIcon(QIcon(":/icons/cute.png"));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut the current selection to the "
                               "clipboard"));
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setIcon(QIcon(":/icons/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy the current selection to the "
                                "clipboard"));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setIcon(QIcon(":/icons/paste.png"));
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Paste the clipboard's contents at "
                                 "the cursor position"));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    closeAction = new QAction(tr("Cl&ose"), this);
    closeAction->setShortcut(QKeySequence::Close);
    closeAction->setStatusTip(tr("Close the active window"));
    connect(closeAction, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAction = new QAction(tr("Close &All"), this);
    closeAllAction->setStatusTip(tr("Close all the windows"));
    connect(closeAllAction, SIGNAL(triggered()), this, SLOT(close()));

    tileAction = new QAction(tr("&Tile"), this);
    tileAction->setStatusTip(tr("Tile the windows"));
    connect(tileAction, SIGNAL(triggered()),
            mdiArea, SLOT(tileSubWindows()));

    cascadeAction = new QAction(tr("&Cascade"), this);
    cascadeAction->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAction, SIGNAL(triggered()),
            mdiArea, SLOT(cascadeSubWindows()));

    nextAction = new QAction(tr("Ne&xt"), this);
    nextAction->setShortcut(QKeySequence::NextChild);
    nextAction->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAction, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAction = new QAction(tr("Pre&vious"), this);
    previousAction->setShortcut(QKeySequence::PreviousChild);
    previousAction->setStatusTip(tr("Move the focus to the previous "
                                    "window"));
    connect(previousAction, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAction = new QAction(this);
    separatorAction->setSeparator(true);

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    windowActionGroup = new QActionGroup(this);


    alignLeftAction=new QAction(tr("align Left"),this);
    alignLeftAction->setStatusTip(tr("Text alignment in left"));
    alignLeftAction->setIcon(QIcon(":/icons/alignLeft.png"));

    alignRightAction=new QAction(tr("align Right"),this);
    alignRightAction->setStatusTip(tr("Text alignment in rigt"));
    alignRightAction->setIcon(QIcon(":/icons/alignRight.png"));

    alignCenterAction=new QAction(tr("align Center"),this);
    alignCenterAction->setStatusTip(tr("Text alignment in center"));
    alignCenterAction->setIcon(QIcon(":/icons/alignCenter"));

    alignJustifyAction=new QAction(tr("align Justify"),this);
    alignJustifyAction->setStatusTip(tr("Text alignment in justify"));
    alignJustifyAction->setIcon(QIcon(":/icons/alignJustify"));

    boldAction=new QAction(tr("bold style"),this);
    boldAction->setStatusTip(tr("to establish a bold print"));
    boldAction->setIcon(QIcon(":/icons/bold.png"));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(boldAction);
    editMenu->addSeparator();
    editMenu->addAction(alignLeftAction);
    editMenu->addAction(alignCenterAction);
    editMenu->addAction(alignJustifyAction);
    editMenu->addAction(alignRightAction);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(closeAction);
    windowMenu->addAction(closeAllAction);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAction);
    windowMenu->addAction(cascadeAction);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAction);
    windowMenu->addAction(previousAction);
    windowMenu->addAction(separatorAction);

    menuBar()->addSeparator();

    viewMenu = menuBar()->addMenu(tr("&View"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);

    fontToolbar=addToolBar(tr("Font"));
    fontToolbar->addAction(boldAction);

    alignmentToolbar=addToolBar(tr("Alignment"));
    alignmentToolbar->addAction(alignLeftAction);
    alignmentToolbar->addAction(alignCenterAction);
    alignmentToolbar->addAction(alignJustifyAction);
    alignmentToolbar->addAction(alignRightAction);

}

void MainWindow::createStatusBar()
{
    readyLabel = new QLabel(tr(" Ready"));
    statusBar()->addWidget(readyLabel, 1);
}

void MainWindow::createConnections()
{
    connect(alignRightAction,&QAction::triggered,
            this,&MainWindow::setRightAlign);
    connect(alignLeftAction,&QAction::triggered,
            this,&MainWindow::setLeftAlign);
    connect(alignCenterAction,&QAction::triggered,
            this,&MainWindow::setCenterAlign);
    connect(alignJustifyAction,&QAction::triggered,
            this,&MainWindow::setJustifyAlign);
    connect(boldAction,&QAction::triggered,
            this,&MainWindow::setBoldStyle);
}

void MainWindow::addEditor(Editor *editor)
{
    connect(editor, SIGNAL(copyAvailable(bool)),
            cutAction, SLOT(setEnabled(bool)));
    connect(editor, SIGNAL(copyAvailable(bool)),
            copyAction, SLOT(setEnabled(bool)));


    QMdiSubWindow *subWindow = mdiArea->addSubWindow(editor);
    windowMenu->addAction(editor->windowMenuAction());
    windowActionGroup->addAction(editor->windowMenuAction());
    subWindow->show();
}

Editor *MainWindow::activeEditor()
{
    QMdiSubWindow *subWindow = mdiArea->activeSubWindow();
    if (subWindow)
        return qobject_cast<Editor *>(subWindow->widget());
    return nullptr;
}

void MainWindow::createDockWin(QWidget *w,const QString nameWindow,Qt::DockWidgetArea area)
{
    QDockWidget *dock=new QDockWidget(nameWindow,this);
    //dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    //    QWidget *e=new Qwidget(dock);

    dock->setWidget(w);
    dock->hide();
    //    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);//прикрепленный виджет не может быть закрыт, перемещен
    addDockWidget(area,dock);


    viewMenu->addAction(dock->toggleViewAction());
}
