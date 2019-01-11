#ifndef SCEDIT_H
#define SCEDIT_H

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>



class Editor : public QMainWindow
{
    Q_OBJECT
public:
    explicit Editor();

public:
    void newFile();
    bool save();
    bool saveAs();
    QSize sizeHint() const;
    QAction *windowMenuAction() const { return action; }

    static Editor *open(QWidget *parent=nullptr);
    static Editor *openFile(const QString &fileName,
                            QWidget *parent=nullptr);

    QsciScintilla *textEdit;
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void documentWasModified();
private:

    bool saveFile(const QString &fileName);
    bool readfile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    bool okToContinue();


    QString curFile;
    QAction *action;
    bool isUntitled;
};

#endif // SCEDIT_H
