#include <QApplication>
#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include "scedit.h"

Editor::Editor()
    : QMainWindow ()
{
    textEdit=new QsciScintilla;
    setCentralWidget(textEdit);
}

void Editor::newFile()
{
    static int documentNumber=1;
    curFile=tr("untitled%1.txt").arg(documentNumber);
    setWindowTitle(curFile+"[*]");
    action->setText(curFile);
    isUntitled=true;
    ++documentNumber;
}

bool Editor::save()
{
    if(isUntitled){
        return saveAs();
    }else {
        return saveFile(curFile);
    }
}

bool Editor::saveAs()
{
    QString fileName=
            QFileDialog::getSaveFileName(this,tr("Save As"),curFile);
    if(fileName.isEmpty())
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
    QString fileName=QFileDialog::getOpenFileName(parent,tr("Open"),".");
    if(fileName.isEmpty())
        return nullptr;
    return openFile(fileName,parent);
}

Editor *Editor::openFile(const QString &fileName, QWidget *parent)
{
    Editor *editor=new Editor();
    if(editor->readfile(fileName)){
        editor->setCurrentFile(fileName);
        return editor;
    } else{
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

bool Editor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)){
        QMessageBox::warning(this,tr("Simple Editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out<<textEdit->text();
    QApplication::restoreOverrideCursor();
    return true;
}

bool Editor::readfile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("Simple Editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setText(in.readAll());
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    return true;
}

void Editor::setCurrentFile(const QString &fileName)
{
    curFile=fileName;
    isUntitled=false;
    action->setText(strippedName(curFile));
    textEdit->setModified(false);
    setWindowTitle(strippedName(curFile)+"[*]");
    setWindowModified(false);
}

QString Editor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool Editor::okToContinue()
{
    if(textEdit->isModified()){
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
