#ifndef __MIKAI_H__
#define __MIKAI_H__

#include <MenuItemInterface.h>
#include <vector>

class Mikai : public MenuItemInterface {
public:
    Mikai() : MenuItemInterface("Mikai") {}
    void drawIcon(float scale);
    void optionsMenu();
    bool hasTheme() { return bruceConfig.theme.mikai; }
    String themePath() { return bruceConfig.theme.paths.mikai; }
};

#endif
