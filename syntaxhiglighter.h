#ifndef SYNTAXHIGLIGHTER_H
#define SYNTAXHIGLIGHTER_H

#include <QSyntaxHighlighter>
class QTextDocument;

class syntaxHiglighter: public QSyntaxHighlighter
{
    Q_OBJECT
private:
    QStringList m_lstKeywords;
protected:
    enum { NormalState=-1, InsideCStyleComment, InsideCString};

    virtual void highlightBlock(const QString&);

    QString getKeyword(int i, const QString& str) const;

public:
    syntaxHiglighter(QTextDocument* parent=nullptr);
};

#endif // SYNTAXHIGLIGHTER_H
