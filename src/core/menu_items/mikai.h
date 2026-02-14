#ifndef __MIKAI_H__
#define __MIKAI_H__

#include <MenuItemInterface.h>
#include <vector>

class mikai : public MenuItemInterface {
public:
    mikai() {}
    String getName() override { return "Mikai"; }
    void optionsMenu() override;
};

#endif
