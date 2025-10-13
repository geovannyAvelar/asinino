/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2025          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C"
{
#endif

  #include "ff.h"

  /* Status of Disk Functions */
  typedef BYTE DSTATUS;

  /* Results of Disk Functions */
  typedef enum
  {
    RES_OK = 0, /* 0: Successful */
    RES_ERROR,  /* 1: R/W Error */
    RES_WRPRT,  /* 2: Write Protected */
    RES_NOTRDY, /* 3: Not Ready */
    RES_PARERR  /* 4: Invalid Parameter */
  } DRESULT;

  /*---------------------------------------*/
  /* Prototypes for disk control functions */
  DSTATUS disk_initialize(BYTE pdrv);
  DSTATUS disk_status(BYTE pdrv);
  DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);
  DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);
  DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);
  uint8_t spi_sd_card_transfer(uint8_t data);
  BYTE send_cmd(BYTE cmd, DWORD arg);
  static int wait_ready(UINT timeout);
  static int rcvr_datablock(BYTE *buff, UINT btr);
  static int xmit_datablock(const BYTE *buff, BYTE token);

  /* SD Card Control registers */
  #define SD_CS_PORT PORTD
  #define SD_CS_DDR DDRD
  #define SD_CS_PIN (1 << PD4)
  #define SD_CS_LOW() (SD_CS_PORT &= ~SD_CS_PIN) // Select card
  #define SD_CS_HIGH() (SD_CS_PORT |= SD_CS_PIN) // Deselect card

  // MMC and SD Card Type Flags
  #define CT_MMC 0x01   /* MMC ver 3 */
  #define CT_SD1 0x02   /* SD ver 1 (older, byte-addressed, simpler init) */
  #define CT_SD2 0x04   /* SD ver 2 (requires CMD8) */
  #define CT_BLOCK 0x08 /* Block addressing (SDHC/SDXC) */

  /* Commands */
  #define CMD0 (0)   /* GO_IDLE_STATE */
  #define CMD1 (1)   /* SEND_OP_COND (MMC) */
  #define CMD8 (8)   /* SEND_IF_COND */
  #define CMD9 (9)   /* SEND_CSD */
  #define CMD12 (12) /* STOP_TRANSMISSION */
  #define CMD17 (17) /* READ_SINGLE_BLOCK */
  #define CMD18 (18) /* READ_MULTIPLE_BLOCK */
  #define CMD24 (24) /* WRITE_BLOCK */
  #define CMD25 (25) /* WRITE_MULTIPLE_BLOCK */
  #define CMD58 (58) /* READ_OCR */

  // Application Command (ACMD) definitions use CMD55 first:
  #define ACMD41 (41) /* SEND_OP_COND (SD) */

  // W5100 Ethernet Chip Select on D10 (PB2) - Keep this HIGH when using SD
  #define ETH_CS_PORT PORTB
  #define ETH_CS_PIN (1 << PB2)
  #define ETH_CS_HIGH() (ETH_CS_PORT |= ETH_CS_PIN)

  /* Disk Status Bits (DSTATUS) */
  #define STA_NOINIT 0x01  /* Drive not initialized */
  #define STA_NODISK 0x02  /* No medium in the drive */
  #define STA_PROTECT 0x04 /* Write protected */

  /* Command code for disk_ioctrl fucntion */

  /* Generic command (Used by FatFs) */
  #define CTRL_SYNC 0        /* Complete pending write process (needed at FF_FS_READONLY == 0) */
  #define GET_SECTOR_COUNT 1 /* Get media size (needed at FF_USE_MKFS == 1) */
  #define GET_SECTOR_SIZE 2  /* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
  #define GET_BLOCK_SIZE 3   /* Get erase block size (needed at FF_USE_MKFS == 1) */
  #define CTRL_TRIM 4        /* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

  /* Generic command (Not used by FatFs) */
  #define CTRL_POWER 5  /* Get/Set power status */
  #define CTRL_LOCK 6   /* Lock/Unlock media removal */
  #define CTRL_EJECT 7  /* Eject media */
  #define CTRL_FORMAT 8 /* Create physical format on the media */

  /* MMC/SDC specific ioctl command (Not used by FatFs) */
  #define MMC_GET_TYPE 10   /* Get card type */
  #define MMC_GET_CSD 11    /* Get CSD */
  #define MMC_GET_CID 12    /* Get CID */
  #define MMC_GET_OCR 13    /* Get OCR */
  #define MMC_GET_SDSTAT 14 /* Get SD status */
  #define ISDIO_READ 55     /* Read data form SD iSDIO register */
  #define ISDIO_WRITE 56    /* Write data to SD iSDIO register */
  #define ISDIO_MRITE 57    /* Masked write data to SD iSDIO register */

  /* ATA/CF specific ioctl command (Not used by FatFs) */
  #define ATA_GET_REV 20   /* Get F/W revision */
  #define ATA_GET_MODEL 21 /* Get model name */
  #define ATA_GET_SN 22    /* Get serial number */

  #ifdef __cplusplus
  }
  #endif

#endif
