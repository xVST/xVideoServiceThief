#ifndef QMACTOOLBAREXT_H
#define QMACTOOLBAREXT_H

#include <QMacToolBar>

class QMacToolBarExt : public QMacToolBar
{
private:
    void buttonAction();
public:
    explicit QMacToolBarExt(QObject *parent = nullptr);
    explicit QMacToolBarExt(const QString &identifier, QObject *parent = nullptr);
    ~QMacToolBarExt();

    void setAsIconAndLabel();
    void setAsOnlyIcon();
    void setAsOnlyLabel();

    void setRegularSizeIcons();
    void setSmallSizeIcons();

    QMacToolBarItem *addButtonItem(const QIcon &icon, const QString &text);

    void setButtonItemSelectable(QMacToolBarItem *item, bool value);
};

#endif // QMACTOOLBAREXT_H
