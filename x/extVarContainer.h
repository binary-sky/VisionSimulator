//存放了单片机上一些不能转移的外部变量 
//例如extern int IntsToSend[10];上位机发送数组
//例如extern float speed;访问虚拟的车速

//请在单片机和电脑上编写不同的"environment.h"和"extVarContainer.h"
//使得文件imageprocess中的代码，可以跨越单片机和电脑，复制粘贴后即可运行在单片机上

#define CAMERA_ROWS 60
#define CAMERA_COLS 80