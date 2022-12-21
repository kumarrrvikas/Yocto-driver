#include <linux/module.h>

int init_module(void)
{
	pr_info("demo_init entered............\n");
	return 0;
}

void cleanup_module(void)
{
	pr_info("demo_exit entered............\n");
}

MODULE_LICENSE("GPL");
