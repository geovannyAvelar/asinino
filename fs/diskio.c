/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Basic definitions of FatFs */
#include "diskio.h" /* Declarations FatFs MAI */
#include "../ds3231.h"
#include "../log.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "../uart.h"

static volatile DSTATUS Stat = STA_NOINIT; /* Disk status */
static BYTE CardType;                      /* Card type flags */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv)
{
  if (Stat == 0)
  {
    return RES_OK;
  }

  return Stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv)
{
  info("Initializing SD card...");

  BYTE n, ocr[4];
  BYTE cmd;

  // 1. HARDWARE INIT (Your original code)
  DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2);             // MOSI, SCK, ETH_CS as output
  SD_CS_DDR |= SD_CS_PIN;                                      // SD_CS as output
  DDRB &= ~(1 << DDB4);                                        // MISO as input
  PORTB |= (1 << PB2);                                         // ETH_CS HIGH (Deselect Ethernet)
  SD_CS_PORT |= SD_CS_PIN;                                     // SD_CS HIGH (Deselect SD)
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0); // SPI Master, F_CPU/128 (SLOW)

  // 2. POWER-UP / IDLE STATE
  // Send 10 dummy clocks (80 clock cycles) with CS HIGH
  for (n = 10; n; n--)
    spi_sd_card_transfer(0xFF);

  // 3. COMMAND SEQUENCE
  // CMD0: GO_IDLE_STATE
  {
    BYTE resp = 0xFF;
    UINT timeout = 200; // ~2s (200 * 10ms)
    do {
      resp = send_cmd(CMD0, 0);
      if (resp == 0x01) break; // R1: Idle State
      _delay_ms(10);
    } while (--timeout);

    if (resp != 0x01)
    {
      error("SD card CMD0 failed (no idle response).");
      return STA_NOINIT;
    } else {
      info("SD card entered Idle State.");
    }
  }

  // CMD8: SEND_IF_COND (Check for SDv2/SDHC)
  if (send_cmd(CMD8, 0x1AA) == 1)
  {
    // SDv2/SDHC card. Read the trailing R7 response.
    for (n = 0; n < 4; n++)
      ocr[n] = spi_sd_card_transfer(0xFF);

    // Wait for card to exit Idle State (ACMD41 with HCS bit)
    n = 200; // Timeout approx 2 seconds (200 * 10ms delay)
    while (n && send_cmd(ACMD41, 1UL << 30))
    { // HCS bit set
      _delay_ms(10);
      n--;
    }

    // Check CCS bit in OCR (for block addressing)
    if (n && send_cmd(CMD58, 0) == 0)
    {
      for (n = 0; n < 4; n++)
        ocr[n] = spi_sd_card_transfer(0xFF);
    }
  }
  else
  {
    // SDv1 or MMC card. (Simpler init logic)
    // Check for SDv1 (ACMD41 without HCS) or MMC (CMD1)
    cmd = (send_cmd(ACMD41, 0) <= 1) ? ACMD41 : CMD1;

    // Wait for card to exit Idle State
    n = 200;
    while (n && send_cmd(cmd, 0))
    {
      _delay_ms(10);
      n--;
    }

    // If timeout, return fail
    if (n == 0)
    {
      error("SD card initialization failed.");
      return STA_NOINIT;
    }
  }

  // Deselect the card after successful initialization
  SD_CS_HIGH();

  // 4. Initialization successful: switch to FAST SPI clock
  // Assuming F_CPU = 16MHz, this sets SCK to F_CPU/4 = 4MHz
  SPCR = (1 << SPE) | (1 << MSTR); // SPR1/SPR0=0
  SPSR = (1 << SPI2X);             // Double speed

  return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,    /* Physical drive number to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
)
{
  // Check if initialization failed
  if (Stat & STA_NOINIT)
    return RES_NOTRDY;

  // Convert LBA sector address to byte address for SDSC cards
  // (If using SDHC, the sector address is already correct.)
  if (!(CardType & CT_BLOCK))
    sector *= 512;

  if (count == 1)
  { // Read Single Block (CMD17)
    if (send_cmd(CMD17, sector) != 0)
      return RES_ERROR; // CMD17 failed

    // Read 512 bytes following the Data Token
    if (rcvr_datablock(buff, 512) != 0)
      return RES_ERROR;
  }
  else
  { // Read Multiple Block (CMD18)
    if (send_cmd(CMD18, sector) != 0)
      return RES_ERROR;

    do
    {
      // Read 512 bytes for each block
      if (rcvr_datablock(buff, 512) != 0)
        break;
      buff += 512;
    } while (--count);

    // Stop transmission after multiple read
    send_cmd(CMD12, 0);
  }

  // Deselect the card and return success
  SD_CS_HIGH();

  return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
    BYTE pdrv,        /* Physical drive number to identify the drive */
    const BYTE *buff, /* Data to be written */
    LBA_t sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to write */
)
{
  // Check for write protect (if implemented)
  if (Stat & STA_PROTECT)
    return RES_WRPRT;
  if (Stat & STA_NOINIT)
    return RES_NOTRDY;

  // Convert LBA address for SDSC cards
  if (!(CardType & CT_BLOCK))
    sector *= 512;

  if (count == 1)
  { // Write Single Block (CMD24)
    if (send_cmd(CMD24, sector) != 0)
      return RES_ERROR;
    if (xmit_datablock(buff, 0xFE) != 0)
      return RES_ERROR; // 0xFE is the token
  }
  else
  { // Write Multiple Block (CMD25)
    if (send_cmd(CMD25, sector) != 0)
      return RES_ERROR;
    do
    {
      // Data Token for multiple block write is 0xFC
      if (xmit_datablock(buff, 0xFC) != 0)
        break;
      buff += 512;
    } while (--count);

    // Stop transmission after multiple write (use 0xFD token)
    xmit_datablock(0, 0xFD);
  }

  // Deselect the card and return success
  SD_CS_HIGH();

  return count ? RES_ERROR : RES_OK;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv, /* Physical drive number to identify the drive */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
)
{
  DRESULT res = RES_ERROR;
  BYTE n;

  if (Stat & STA_NOINIT)
    return RES_NOTRDY;

  SD_CS_LOW(); // Select the card

  switch (cmd)
  {
  case CTRL_SYNC: // Flush pending write process
    if (wait_ready(3000) == 0)
      res = RES_OK; // Wait for card to not be busy
    break;

  case GET_SECTOR_COUNT: // Get total sector count
    // Requires sending CMD9 (CSD) or CMD10 (CID) and parsing the data
    if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(buff, 16))
    {
      // Logic to parse CSD and calculate total sectors goes here
      // ...
      res = RES_OK;
    }
    break;

  case GET_SECTOR_SIZE: // Get sector size (always 512 for standard SD)
    *(DWORD *)buff = 512;
    res = RES_OK;
    break;

  default:
    res = RES_PARERR; // Parameter error
  }

  SD_CS_HIGH(); // Deselect the card
  return res;
}

uint8_t spi_sd_card_transfer(uint8_t data)
{
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}

BYTE send_cmd(BYTE cmd, DWORD arg)
{
  BYTE n, res;

  // 1. Deselect if currently selected (to ensure fresh transaction)
  SD_CS_HIGH();
  spi_sd_card_transfer(0xFF); // Provide a short delay

  // 2. Select the card, and keep Ethernet deselected
  SD_CS_LOW();
  ETH_CS_HIGH(); // Essential when using a shared bus shield!

  // 3. Send command (CMD) packet (6 bytes)
  spi_sd_card_transfer(cmd | 0x40);        // Start byte (0x40) + Command Index
  spi_sd_card_transfer((BYTE)(arg >> 24)); // Argument bytes 1-4
  spi_sd_card_transfer((BYTE)(arg >> 16));
  spi_sd_card_transfer((BYTE)(arg >> 8));
  spi_sd_card_transfer((BYTE)arg);

  // 4. Send CRC (dummy CRC for now, except for CMD0 and CMD8)
  n = 0x01;
  if (cmd == CMD0)
    n = 0x95; // CRC for CMD0
  if (cmd == CMD8)
    n = 0x87; // CRC for CMD8
  spi_sd_card_transfer(n);

  // 5. Wait for response (R1) - Max 8 attempts
  for (n = 10; n; n--)
  {
    res = spi_sd_card_transfer(0xFF);
    if (!(res & 0x80))
      break; // Found response (R1 is not 0xFF)
  }

  return res;
}

static int wait_ready(UINT timeout)
{
  BYTE d;

  // Convert timeout (in ms) to loop count for the delay loop
  // Note: The actual timing depends on your delay function resolution.
  UINT i = timeout * 100; // Example: 300ms timeout * 100 cycles per ms = 30000 cycles

  // Select the card if it's not already selected (good practice)
  SD_CS_LOW();

  // Poll the MISO line (send 0xFF to clock out the response)
  do
  {
    d = spi_sd_card_transfer(0xFF);

    // Check if the card is ready (MISO is 0xFF)
    if (d == 0xFF)
    {
      SD_CS_HIGH(); // Deselect card
      return 0;     // Card is ready
    }
  } while (--i);

  SD_CS_HIGH(); // Deselect card on timeout
  return 1;     // Timeout failure
}

// Requires: spi_sd_card_transfer() and SD_CS_HIGH()/SD_CS_LOW() macros
// Returns 0 on success, 1 on error/timeout.
static int rcvr_datablock(BYTE *buff, UINT btr)
{
  BYTE token;
  UINT timer;

  // 1. Wait for Start Block Token (0xFE)
  timer = 20000; // Timeout approx 200ms (adjust based on your F_CPU and loop timing)
  do
  {
    token = spi_sd_card_transfer(0xFF);
    if (token != 0xFF)
      break; // Token received or error byte received
  } while (timer--);

  // Check for timeout
  if (token == 0xFF)
  {
    SD_CS_HIGH(); // Deselect card
    return 1;     // Timeout failure
  }

  // Check if the token is the expected Start Block Token
  if (token != 0xFE)
  {
    SD_CS_HIGH(); // Deselect card
    return 1;     // Error: Unexpected token
  }

  // 2. Receive the Data Block (512 bytes)
  do
  {
    // Read one byte and store it in the buffer
    *buff++ = spi_sd_card_transfer(0xFF);
  } while (--btr);

  // 3. Receive the 2-byte CRC (not used, but must be clocked out)
  spi_sd_card_transfer(0xFF); // CRC Low Byte
  spi_sd_card_transfer(0xFF); // CRC High Byte

  // 4. Deselect and return success
  // Note: The disk_read function typically handles the final SD_CS_HIGH()
  // and cleanup after reading, but it is safe to do here if needed.

  return 0; // Success
}

// Requires: spi_sd_card_transfer(), wait_ready()
// Returns 0 on success, 1 on error/timeout.
static int xmit_datablock(const BYTE *buff, BYTE token)
{
  BYTE resp;
  UINT wc;

  // 1. Send the Start Block Token
  // 'token' will be 0xFE (single block write) or 0xFC (multiple block write data)
  spi_sd_card_transfer(token);

  // If the token is 0xFD (Stop Transmission token for multi-write),
  // we skip the data transfer part.
  if (token != 0xFD)
  {

    // 2. Send 512 Data Bytes
    wc = 512;
    do
    {
      spi_sd_card_transfer(*buff++);
    } while (--wc);

    // 3. Send 2-byte Dummy CRC (The card ignores this in SPI mode)
    spi_sd_card_transfer(0xFF); // CRC Low Byte
    spi_sd_card_transfer(0xFF); // CRC High Byte

    // 4. Wait for Data Response Token
    // Read until the card responds (max 8 clock cycles)
    resp = spi_sd_card_transfer(0xFF);

    // Check the three-bit status field (bits 0, 1, 2)
    // 0x05 = Data Accepted (Success)
    if ((resp & 0x1F) != 0x05)
    {
      return 1; // Write error
    }

    // 5. Wait for the card to finish writing to flash memory
    // This is necessary after the card accepts the data.
    if (wait_ready(500) != 0)
    {           // Using your wait_ready implementation
      return 1; // Timeout failure
    }
  }

  return 0; // Success
}

DWORD get_fattime(void)
{
    ds3231_time_t now;
    ds3231_read_time(&now);
    
    // The DS3231 stores the year as 0-99 (e.g., 25 for 2025). 
    // FAT requires the year relative to 1980 (e.g., 2025 -> 45).
    // The year from the DS3231 (e.g., 25) typically needs 
    // to be adjusted to include the century and then made relative to 1980.

    // Assuming your DS3231 implementation returns 25 for 2025:
    uint16_t current_year = 2000 + now.year;
    
    // 1. Calculate the FAT Year (relative to 1980)
    uint8_t fat_year = current_year - 1980;

    // 2. Pack the data into a 32-bit DWORD (Date: Bits 31-16, Time: Bits 15-0)
    DWORD fattime = 0;

    // Date part (Bits 31-16)
    fattime |= ((DWORD)fat_year         << 25); // 7 bits: Year (0-127) relative to 1980
    fattime |= ((DWORD)now.month        << 21); // 4 bits: Month (1-12)
    fattime |= ((DWORD)now.day_of_month << 16); // 5 bits: Day (1-31)

    // Time part (Bits 15-0)
    fattime |= ((DWORD)now.hour         << 11); // 5 bits: Hour (0-23)
    fattime |= ((DWORD)now.min          << 5);  // 6 bits: Minute (0-59)
    fattime |= ((DWORD)now.sec          >> 1);  // 5 bits: Second/2 (0-29)

    return fattime;
}