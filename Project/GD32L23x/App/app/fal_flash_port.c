#include <fal.h>

/*********************************外部flash接口*********************************/
static int external_init(void);
static int external_read(long offset, uint8_t *buf, size_t size);
static int external_write(long offset, const uint8_t *buf, size_t size);
static int external_erase(long offset, size_t size);

struct fal_flash_dev external_flash_device = {
	.name = NOR_FLASH_DEV_NAME, //名字
	.addr = 0, //起始地址
	.len = 8 * 1024 * 1024, // 8粒度 * 1M
	.blk_size = SECTOR_SISE, //4k 扇区
	.ops = { external_init, external_read, external_write, external_erase }, //回调函数
	.write_gran = 1,
};

static int external_init(void)
{
	return 0;
}

static int external_read(long offset, uint8_t *buf, size_t size)
{
	return 0;
}

static int external_write(long offset, const uint8_t *buf, size_t size)
{
	return 0;
}

static int external_erase(long offset, size_t size)
{
	return 0;
}

/*********************************外部flash接口*********************************/

/*********************************内部flash接口*********************************/
static int internal_init(void);
static int internal_read(long offset, uint8_t *buf, size_t size);
static int internal_write(long offset, const uint8_t *buf, size_t size);
static int internal_erase(long offset, size_t size);
struct fal_flash_dev internal_flash_device = {
	.name = MCU_FLASH_DEV_NAME, //名字
	.addr = 0x08000000, //起始地址
	.len = 256 * 1024, // 256k 字节
	.blk_size = MCU_PAGE_SIZE, //MCU 一页 4k
	.ops = { internal_init, internal_read, internal_write, internal_erase }, //回调函数
	.write_gran = 32, //手册建议写字
};

static int internal_init(void)
{
	return 0;
}

static int internal_read(long offset, uint8_t *buf, size_t size)
{
	return 0;
}

static int internal_write(long offset, const uint8_t *buf, size_t size)
{
	return 0;
}

static int internal_erase(long offset, size_t size)
{
	return 0;
}

/*********************************内部flash接口*********************************/