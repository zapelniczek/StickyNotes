#include "phrasehighlighter.h"

PhraseHighlighter::PhraseHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{

}

void PhraseHighlighter::highlightBlock(const QString &text)
{
    if(searchedPhrase.isEmpty())
        return;

    QTextCharFormat charFormat;
    charFormat.setBackground(QBrush(0x00FFFF));

    for (int startPos = 0; startPos < text.count(); )
    {
        int pos = text.indexOf(searchedPhrase, startPos);

        if(pos == -1)
           return;

        setFormat(pos, searchedPhrase.count(), charFormat);

        startPos = pos + searchedPhrase.count();
    }
}

void PhraseHighlighter::setPhraseToLook(const QString& phrase)
{
    searchedPhrase = phrase;
}

