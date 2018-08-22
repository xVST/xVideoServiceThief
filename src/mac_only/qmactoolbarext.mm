#import <AppKit/AppKit.h>

#import <QtMac>

#include "qmactoolbarext.h"
#include "NSButtonToolbar.h"

#include "../tools.h"

QMacToolBarExt::QMacToolBarExt(QObject *parent) : QMacToolBar(parent)
{
}

QMacToolBarExt::QMacToolBarExt(const QString &identifier, QObject *parent) : QMacToolBar(identifier, parent)
{
}

QMacToolBarExt::~QMacToolBarExt()
{
}

void QMacToolBarExt::setAsIconAndLabel()
{
    [this->nativeToolbar() setDisplayMode:NSToolbarDisplayModeIconAndLabel];
}

void QMacToolBarExt::setAsOnlyIcon()
{
    [this->nativeToolbar() setDisplayMode:NSToolbarDisplayModeIconOnly];
}

void QMacToolBarExt::setAsOnlyLabel()
{
    [this->nativeToolbar() setDisplayMode:NSToolbarDisplayModeLabelOnly];
}

void QMacToolBarExt::setRegularSizeIcons()
{
    [this->nativeToolbar() setSizeMode:NSToolbarSizeModeRegular];
}

void QMacToolBarExt::setSmallSizeIcons()
{
    [this->nativeToolbar() setSizeMode:NSToolbarSizeModeSmall];
}

QMacToolBarItem* QMacToolBarExt::addButtonItem(const QIcon &icon, const QString &text)
{
    QMacToolBarItem *item = addItem(QIcon(), text);
    // prepare the pixmap to be retina (if needed)
    int devicePixelRatio = QApplication::desktop()->devicePixelRatio();
    QPixmap pixmap = icon.pixmap(16*devicePixelRatio, 16*devicePixelRatio);
    // convert the pixmap to NSImage
    CGImageRef cgimage = QtMac::toCGImageRef(pixmap);
    NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:cgimage];
    NSImage *image = [[NSImage alloc] initWithSize:CGSizeMake(16, 16)];
    [image addRepresentation:bitmapRep];
    [bitmapRep release];
    CFRelease(cgimage);
    // create the button
    NSButtonToolbar *button = [[NSButtonToolbar alloc] initWithFrame:CGRectMake(0, 0, 42, 25)];
    [button setItemIdentifier:item->nativeToolBarItem().itemIdentifier];
    [button setBezelStyle:NSBezelStyleTexturedRounded];
    [button setImage:image];
    [button setEnabled:item->selectable()];
    [item->nativeToolBarItem() setView:button];
    [button release];
    // return the new item
    return item;
}

void QMacToolBarExt::setButtonItemSelectable(QMacToolBarItem *item, bool value)
{
    if (item)
    {
        // update the item selectable property
        item->setSelectable(value);
        // update the button
        reinterpret_cast<NSButton *>(item->nativeToolBarItem().view).enabled = value;
    }
}
