#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>

class QColorDialog;

class Editor : public QTextEdit
{
    Q_OBJECT

public:
    Editor(QWidget *parent = nullptr);

    void newFile();
    bool save();
    bool saveAs();
    QSize sizeHint() const;
    QAction *windowMenuAction() const { return action; }

    static Editor *open(QWidget *parent = nullptr);
    static Editor *openFile(const QString &fileName,
                            QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void documentWasModified();
    void setBold(bool on);
    void setColor();
    void updateColor(const QColor &color);
    void setFontPointSize(double points);
    void setFontFamily(const QFont &font);
//    void currentCharFormatChanged(const QTextCharFormat &format);

    void alignLeft();
    void alignRight();
    void alignCenter();
    void alignJustify();

//    void cursorPositionChanged(); //todo перенсти в mainwin.cpp|hpp


private:
    bool okToContinue();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

//    void updateColorSwatch(); //todo перенсти в mainwin.cpp|hpp

    QString curFile;
    bool isUntitled;
    QAction *action;


    QColorDialog *colorDialog;
};

#endif
