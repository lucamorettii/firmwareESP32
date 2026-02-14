#ifndef __MIKAI_H__
#define __MIKAI_H__

#include <MenuItemInterface.h>
#include <vector>

class mikai : public MenuItemInterface {
public:
    mikai() : MenuItemInterface("Mikai") {}
    void drawIcon(float scale);
    void optionsMenu() override;
};

#endif
