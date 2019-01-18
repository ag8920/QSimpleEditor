#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>

class QColorDialog;
class QCompleter;
class QStringListModel;

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
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
public slots:
    void documentWasModified();
    void setBold(bool on);
    void setColor();
    void updateColor(const QColor &color);
    void setFontSize(double points);
    void setFont(const QFont &font);
//    void currentCharFormatChanged(const QTextCharFormat &format);

    void alignLeft();
    void alignRight();
    void alignCenter();
    void alignJustify();

//    void cursorPositionChanged(); //TODO перенсти в mainwin.cpp|hpp
private slots:
    void insertCompletion(const QString &completion,
                          bool singleWord=false);
    void performCompletion();

private:
    void createConnections();
    bool okToContinue();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    void performCompletion(const QString &completionPrefix);
    void populateModel(const QString &completionPrefix);
    bool handledCompletedAndSelected(QKeyEvent *event);
//    void updateColorSwatch(); //TODO перенсти в mainwin.cpp|hpp

    QString curFile;
    bool isUntitled;
    QAction *action;

    QCompleter *completer;
    QStringListModel *model;

    QColorDialog *colorDialog;

    bool completeAndSelected;
};

#endif
