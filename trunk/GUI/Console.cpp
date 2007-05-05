#include "Console.h"

QConsole::QConsole(QWidget *parent)
: QTextEdit(parent)
{
     setReadOnly(true);
}

void QConsole::Display_Messages(vector<string> VM_in)
{
    for (unsigned int x=0; x<VM_in.size(); x++)
    {
        append(QString(VM_in[x].c_str()));
    }
}
