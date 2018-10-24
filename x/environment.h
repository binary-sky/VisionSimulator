//指示了当前的环境
//请在单片机和电脑上编写不同的"environment.h"和"extVarContainer.h"
//使得文件imageprocess中的代码，可以跨越单片机和电脑，复制粘贴后即可运行在单片机上
#define RUN_ON_VS		//指示当前环境是电脑

//#define RUN_ON_CHIP	//指示当前环境是单片机