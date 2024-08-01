#include "app_init.h"

int main(void)
{
	app_task_init();

	task_dispatch_looper();
}
