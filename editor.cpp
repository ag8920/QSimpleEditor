#include "editor.h"
#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QTextEdit>
#include <QApplication>
#include <QColorDialog>

#include "syntaxhiglighter.h"

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

void Editor::setFontPointSize(double points)
{
    this->setFontPointSize(static_cast<qreal>(points));
}

void Editor::setFontFamily(const QFont &font)
{
    this->setFontFamily(font.family());
}


void Editor::alignLeft(){this->setAlignment(Qt::AlignLeft);}

void Editor::alignRight(){this->setAlignment(Qt::AlignRight);}

void Editor::alignCenter(){this->setAlignment(Qt::AlignCenter);}

void Editor::alignJustify(){this->setAlignment(Qt::AlignJustify);}


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





