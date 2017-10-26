Networking 默认为是Header only的，如果要编译为库
需要#include "Networking/detail/src.hpp"
如果是编译为动态库，还需要在编译器设置中添加 NETWORK_DYNAMIC_LINK 宏

Macro                           Description
NETWORK_DYNAMIC_LINK            将Networking编译为动态库