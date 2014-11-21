#ifndef _IAP_H_INCLUDED
#define _IAP_H_INCLUDED

#ifdef __cplusplus__
extern "C" {
#endif /* __cplusplus__ */

#define IAP_CMD_SIZE	(5)
#define IAP_RES_SIZE	(3)

#define IAP_FLASH_SECTOR	(0x10)
#define IAP_FLASH_BEGIN		((void *)(0x3C000))
#define IAP_FLASH_SIZE		(0x2000)

#define IAP_PREPARE_SECTORS_FOR_WRITE	(50)
#define IAP_COPY_RAM_TO_FLASH			(51)
#define IAP_ERASE_SECTORS				(52)
#define IAP_BLANK_CHECK_SECTORS			(53)
#define IAP_READ_PART_ID				(54)
#define IAP_READ_BOOT_CODE_VERSION		(55)
#define	IAP_COMPARE						(56)

#define	IAP_CMD_SUCCESS			(0)
#define	IAP_INVALID_COMMAND		(1)
#define	IAP_SRC_ADDR_ERROR		(2)
#define	IAP_DST_ADDR_ERROR		(3)
#define	IAP_SRC_ADDR_NOT_MAPPED	(4)
#define	IAP_DST_ADDR_NOT_MAPPED	(5)	
#define	IAP_COUNT_ERROR			(6)
#define	IAP_INVALID_SECTOR		(7)
#define	IAP_SECTOR_NOT_BLANK	(8)	
#define	IAP_SECTOR_NOT_PREPARED	(9)
#define	IAP_COMPARE_ERROR		(10)
#define	IAP_BUSY				(11)

/*
unsigned int iap_cmd[IAP_CMD_SIZE];
unsigned int iap_res[IAP_RES_SIZE];

typedef void (* IAP)(unsigned int [], unsigned int []);
IAP iap_proc=(IAP)0x7FFFFFF1;
*/

int iap_copy_ram_to_flash(void * /* buf */, int /* size */);
int iap_clear_flash(void);
int iap_compare(void * /* adr1 */, void * /* adr2 */, int /* size */);

#ifdef __cplusplus__
}
#endif /* __cplusplus__ */

#endif /* _IAP_H_INCLUDED */
