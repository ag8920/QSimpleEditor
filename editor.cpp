#include "editor.h"
#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QTextEdit>
#include <QApplication>
#include <QColorDialog>
#include <QCompleter>
#include <QStringListModel>
#include <QShortcut>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QPlainTextEdit>

#include "syntaxhiglighter.h"

inline
bool caseInsensitiveLessThan(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) < 0;
}


Editor::Editor(QWidget *parent)
    : QTextEdit(parent),colorDialog(0)
{
    action = new QAction(this);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(show()));
    connect(action, SIGNAL(triggered()), this, SLOT(setFocus()));

    isUntitled = true;

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    setWindowIcon(QPixmap(":/images/document.png"));
    setWindowTitle("[*]");
    setAttribute(Qt::WA_DeleteOnClose);

    new syntaxHiglighter(this->document());
    QPalette pal=this->palette();
    //    pal.setColor(QPalette::Base,Qt::darkBlue);
    //    pal.setColor(QPalette::Text,Qt::yellow);
    this->setPalette(pal);

    //автозавершение текста
    model=new QStringListModel(this);
    completer=new QCompleter(this);
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion); //раскрывающийся список
    completer->setModel(model);
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);

    createConnections();

}

void Editor::newFile()
{
    static int documentNumber = 1;

    curFile = tr("untitled%1.txt").arg(documentNumber);
    setWindowTitle(curFile + "[*]");
    action->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}

bool Editor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool Editor::saveAs()
{
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

QSize Editor::sizeHint() const
{
    return QSize(72 * fontMetrics().width('x'),
                 25 * fontMetrics().lineSpacing());
}

Editor *Editor::open(QWidget *parent)
{
    QString fileName =
            QFileDialog::getOpenFileName(parent, tr("Open"), ".");
    if (fileName.isEmpty())
        return nullptr;

    return openFile(fileName, parent);
}

Editor *Editor::openFile(const QString &fileName, QWidget *parent)
{
    Editor *editor = new Editor(parent);
    if (editor->readFile(fileName)) {
        editor->setCurrentFile(fileName);
        return editor;
    } else {
        delete editor;
        return nullptr;
    }
}

void Editor::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void Editor::documentWasModified()
{
    setWindowModified(true);
}
//------------------------------------------------------------
//форматирование текста
//------------------------------------------------------------
void Editor::setBold(bool on)
{
    this->setFontWeight(on?QFont::Bold : QFont::Normal);
}

void Editor::setColor()
{
    if(!colorDialog){
        colorDialog = new QColorDialog(this);
        connect(colorDialog,SIGNAL(colorSelected(const QColor&)),
                this,SLOT(updateColor(const QColor&)));
    }
    colorDialog->setCurrentColor(this->textColor());
    colorDialog->open();
}

void Editor::updateColor(const QColor &color)
{
    this->setTextColor(color);
    //    updateColorSwatchSignal(); //todo сделать сигналом
}

//void Editor::updateColorSwatch()//todo перенести в mainwin.cpp|hpp
//{

//}

void Editor::setFontSize(double points)
{
    this->setFontPointSize(static_cast<qreal>(points));
}

void Editor::setFont(const QFont &font)
{
    this->setFontFamily(font.family());
}
//------------------------------------------------------------
//выравнивание текста
//------------------------------------------------------------
void Editor::alignLeft(){this->setAlignment(Qt::AlignLeft);}

void Editor::alignRight(){this->setAlignment(Qt::AlignRight);}

void Editor::alignCenter(){this->setAlignment(Qt::AlignCenter);}

void Editor::alignJustify(){this->setAlignment(Qt::AlignJustify);}
//------------------------------------------------------------
//автозавершение текста
//------------------------------------------------------------
void Editor::insertCompletion(const QString &completion,
                              bool singleWord)
{
    QTextCursor cursor=textCursor();
    int numberofCharsToComplete=completion.length() -
            completer->completionPrefix().length();
    int insertionPosition=cursor.position();
    cursor.insertText(completion.right(numberofCharsToComplete));
    if(singleWord){
        cursor.setPosition(insertionPosition);
        cursor.movePosition(QTextCursor::EndOfWord,
                            QTextCursor::KeepAnchor);
        completeAndSelected=true;
    }
    setTextCursor(cursor);
}

void Editor::performCompletion()
{
    QTextCursor cursor=textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString completionPrefix=cursor.selectedText();
    if(!completionPrefix.isEmpty() &&
            completionPrefix.at(completionPrefix.length()-1).isLetter())
        performCompletion(completionPrefix);
}
void Editor::performCompletion(const QString &completionPrefix)
{
    populateModel(completionPrefix);
    if(completionPrefix!=completer->completionPrefix()){
        completer->setCompletionPrefix(completionPrefix);

        completer->popup()->
                setCurrentIndex(completer->completionModel()->index(0,0));
    }
    if(completer->completionCount()==1)
        insertCompletion(completer->currentCompletion(),true);
    else {
        QRect rect=cursorRect();
        rect.setWidth(completer->popup()->sizeHintForColumn(0)+
                      completer->popup()->verticalScrollBar()->
                      sizeHint().width());
        completer->complete(rect);
    }
}

//метод динамически заполняет модель механизма автозавершения
//словами из текущего текста
void Editor::populateModel(const QString &completionPrefix)
{
    QStringList strings=toPlainText().split(QRegExp("\\W+"));
    strings.removeAll(completionPrefix);
    strings.removeDuplicates();
    qSort(strings.begin(),strings.end(),caseInsensitiveLessThan);
    model->setStringList(strings);
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    if(completeAndSelected){
        completeAndSelected=false;
        QTextCursor cursor=textCursor();
        cursor.removeSelectedText();
        setTextCursor(cursor);
    }
    //    QPlainTextEdit::mousePressEvent(event); todo
    QTextEdit::mousePressEvent(event);
}

void Editor::keyPressEvent(QKeyEvent *event)
{
    if (completeAndSelected && handledCompletedAndSelected(event) )
        return;
    completeAndSelected=false;
    if(completer->popup()->isVisible()){
        switch (event->key()) {
        case Qt::Key_Up:      // Fallthrough
        case Qt::Key_Down:    // Fallthrough
        case Qt::Key_Enter:   // Fallthrough
        case Qt::Key_Return:  // Fallthrough
        case Qt::Key_Escape: event->ignore(); return;
        default: completer->popup()->hide(); break;
        }
    }
    //    QPlainTextEdit::keyPressEvent(event); todo
    QTextEdit::keyPressEvent(event);
}

bool Editor::handledCompletedAndSelected(QKeyEvent *event)
{
    completeAndSelected=false;
    QTextCursor cursor=textCursor();
    switch (event->key()) {
    case Qt::Key_Enter:  // Fallthrough
    case Qt::Key_Return: cursor.clearSelection(); break;
    case Qt::Key_Escape: cursor.removeSelectedText(); break;
    default: return false;
    }
    setTextCursor(cursor);
    event->accept();
    return true;
}
//------------------------------------------------------------




void Editor::createConnections()
{
    connect(completer, SIGNAL(activated(const QString&)),
            this, SLOT(insertCompletion(const QString&)));
    (void) new QShortcut(QKeySequence(tr("Ctrl+M", "Complete")),
                         this,SLOT(performCompletion()));
}


//перенести в mainwin
//void Editor::cursorPositionChanged()
//{
//    QTextCursor cursor=this->textCursor();
//    QTextBlockFormat format=cursor.blockFormat();
//    switch (format.alignment()) {
//        case Qt::AlignLeft:
//        alignLeft
//    }
//}

bool Editor::okToContinue()
{
    if (document()->isModified()) {
        int r = QMessageBox::warning(this, tr("Simple Editor"),
                                     tr("File %1 has been modified.\n"
                                        "Do you want to save your changes?")
                                     .arg(strippedName(curFile)),
                                     QMessageBox::Yes | QMessageBox::No
                                     | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool Editor::saveFile(const QString &fileName)
{
    if (writeFile(fileName)) {
        setCurrentFile(fileName);
        return true;
    } else {
        return false;
    }
}

void Editor::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    isUntitled = false;
    action->setText(strippedName(curFile));
    document()->setModified(false);
    setWindowTitle(strippedName(curFile) + "[*]");
    setWindowModified(false);
}

bool Editor::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Simple Editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
    return true;
}

bool Editor::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Simple Editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();
    return true;
}

QString Editor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}






