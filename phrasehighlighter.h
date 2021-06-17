#ifndef PHRASEHIGHLIGHTER_H
#define PHRASEHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class PhraseHighlighter : public QSyntaxHighlighter
{
public:
    explicit PhraseHighlighter(QTextDocument *parent);

    void highlightBlock(const QString &text) override;

public slots:
    void setPhraseToLook(const QString&);

private:
    QString searchedPhrase;
};

#endif // PHRASEHIGHLIGHTER_H
