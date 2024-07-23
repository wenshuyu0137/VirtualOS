/* ===================== Flash device Configuration ========================= */

#ifndef _FAL_CGH_H
#define _FAL_CGH_H

#define SECTOR_SISE (4 * 1024) //扇区大小
#define NOR_FLASH_DEV_NAME "GD25Q80E"

#define MCU_PAGE_SIZE (4 * 1024) //4k一页
#define MCU_FLASH_DEV_NAME "GD32_FLASH"

#define FAL_PART_HAS_TABLE_CFG
#ifdef FAL_PART_HAS_TABLE_CFG

/* flash 设备表 新建分区时要新添加一个设备*/
extern struct fal_flash_dev external_flash_device; //外部FLASH设备
extern struct fal_flash_dev internal_flash_device; //MCU FLASH设备

#define FAL_FLASH_DEV_TABLE { &external_flash_device, &internal_flash_device }

/******内部FLASH分区*******/
#define BOOTLOADER_PARTITION_NAME "bl"
#define BOOTLOADER_PARTITION_OFFSET 0
#define BOOTLOADER_PARTITION_SIZE (32 * 1024) //32K

#define APP_PARTITION_NAME "app"
#define APP_PARTITION_OFFSET (BOOTLOADER_PARTITION_OFFSET + BOOTLOADER_PARTITION_SIZE)
#define APP_PARTITION_SIZE (128 * 1024) //128k

#define FLAG_PARTITION_NAME "flag"
#define FLAG_PARTITION_OFFSET (APP_PARTITION_OFFSET + APP_PARTITION_SIZE)
#define FLAG_PARTITION_SIZE MCU_PAGE_SIZE

/******外部FLASH分区*******/

/*经测试最小需要两个扇区*/

//BMS参数分区
#define BMS_ARGS_PARTITION_NAME "bms_argvs"
#define BMS_ARGS_PARTITION_OFFSET 0
#define BMS_ARGS_PARTITION_SIZE (8 * SECTOR_SISE)

//故障记录记录分区
#define FAULT_RECORD_PARTITION_NAME "fault_record"
#define FAULT_RECORD_PARTITION_OFFSET (BMS_ARGS_PARTITION_OFFSET + BMS_ARGS_PARTITION_SIZE)
#define FAULT_RECORD_PARTITION_SIZE (16 * SECTOR_SISE)

//充放电记录分区
#define CHG_DSG_RECORD_PARTITION_NAME "chg_dsg_record"
#define CHG_DSG_RECORD_PARTITION_OFFSET (FAULT_RECORD_PARTITION_OFFSET + FAULT_RECORD_PARTITION_SIZE)
#define CHG_DSG_RECORD_PARTITION_SIZE (16 * SECTOR_SISE)

//极值纪录
#define EXTREM_RECORD_PARTITION_NAME "extrem_record"
#define EXTREM_RECORD_PARTITION_OFFSET (CHG_DSG_RECORD_PARTITION_OFFSET + CHG_DSG_RECORD_PARTITION_SIZE)
#define EXTREM_RECORD_PARTITION_SIZE (2 * SECTOR_SISE)

//周期记录分区
#define PERIOD_RECORD_PARTITION_NAME "period_record"
#define PERIOD_RECORD_PARTITION_OFFSET (EXTREM_RECORD_PARTITION_OFFSET + EXTREM_RECORD_PARTITION_SIZE)
#define PERIOD_RECORD_PARTITION_SIZE (16 * SECTOR_SISE)

//app下载区域
#define APP_DOWNLOAD_PARTITION_NAME "app_download"
#define APP_DOWNLOAD_PARTITION_OFFSET (PERIOD_RECORD_PARTITION_OFFSET + PERIOD_RECORD_PARTITION_SIZE)
#define APP_DOWNLOAD_PARTITION_SIZE (APP_PARTITION_SIZE)

//soc值区域
#define SOC_PARTITION_NAME "soc"
#define SOC_PARTITION_OFFSET (APP_DOWNLOAD_PARTITION_OFFSET + APP_DOWNLOAD_PARTITION_SIZE)
#define SOC_PARTITION_SIZE (2 * SECTOR_SISE)

/* ====================== 分区配置 ========================== */
/*FAL_PART_MAGIC_WORD(固定值)        分区名            Flash 设备名             偏移地址       结束地址(建议至少两个扇区8k)    0(固定值) */
#define FAL_PART_TABLE                                                                                                                                         \
	{                                                                                                                                                      \
		{ FAL_PART_MAGIC_WORD, BOOTLOADER_PARTITION_NAME, MCU_FLASH_DEV_NAME, BOOTLOADER_PARTITION_OFFSET,                                             \
		  (BOOTLOADER_PARTITION_OFFSET + BOOTLOADER_PARTITION_SIZE), 0 },                                                                              \
		{ FAL_PART_MAGIC_WORD, APP_PARTITION_NAME, MCU_FLASH_DEV_NAME, APP_PARTITION_OFFSET, (APP_PARTITION_OFFSET + APP_PARTITION_SIZE), 0 },         \
		{ FAL_PART_MAGIC_WORD, FLAG_PARTITION_NAME, MCU_FLASH_DEV_NAME, FLAG_PARTITION_OFFSET, (FLAG_PARTITION_OFFSET + FLAG_PARTITION_SIZE), 0 },     \
		{ FAL_PART_MAGIC_WORD, BMS_ARGS_PARTITION_NAME, NOR_FLASH_DEV_NAME, BMS_ARGS_PARTITION_OFFSET,                                                 \
		  (BMS_ARGS_PARTITION_OFFSET + BMS_ARGS_PARTITION_SIZE), 0 },                                                                                  \
		{ FAL_PART_MAGIC_WORD, FAULT_RECORD_PARTITION_NAME, NOR_FLASH_DEV_NAME, FAULT_RECORD_PARTITION_OFFSET,                                         \
		  (FAULT_RECORD_PARTITION_OFFSET + FAULT_RECORD_PARTITION_SIZE), 0 },                                                                          \
		{ FAL_PART_MAGIC_WORD, CHG_DSG_RECORD_PARTITION_NAME, NOR_FLASH_DEV_NAME, CHG_DSG_RECORD_PARTITION_OFFSET,                                     \
		  (CHG_DSG_RECORD_PARTITION_OFFSET + CHG_DSG_RECORD_PARTITION_SIZE), 0 },                                                                      \
		{ FAL_PART_MAGIC_WORD, EXTREM_RECORD_PARTITION_NAME, NOR_FLASH_DEV_NAME, EXTREM_RECORD_PARTITION_OFFSET,                                       \
		  (EXTREM_RECORD_PARTITION_OFFSET + EXTREM_RECORD_PARTITION_SIZE), 0 },                                                                        \
		{ FAL_PART_MAGIC_WORD, PERIOD_RECORD_PARTITION_NAME, NOR_FLASH_DEV_NAME, PERIOD_RECORD_PARTITION_OFFSET,                                       \
		  (PERIOD_RECORD_PARTITION_OFFSET + PERIOD_RECORD_PARTITION_SIZE), 0 },                                                                        \
		{ FAL_PART_MAGIC_WORD, APP_DOWNLOAD_PARTITION_NAME, NOR_FLASH_DEV_NAME, APP_DOWNLOAD_PARTITION_OFFSET,                                         \
		  (APP_DOWNLOAD_PARTITION_OFFSET + APP_DOWNLOAD_PARTITION_SIZE), 0 },                                                                          \
		{ FAL_PART_MAGIC_WORD, SOC_PARTITION_NAME, NOR_FLASH_DEV_NAME, SOC_PARTITION_OFFSET, (SOC_PARTITION_OFFSET + SOC_PARTITION_SIZE), 0 },         \
	}

#endif
#endif