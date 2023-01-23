#include "Settings.h"

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "stdio.h"

Settings *Settings::shared;

void Settings::init()
{
    Settings::shared = new Settings();
    Settings::shared->read();
}

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

void Settings::read()
{
    int page = this->getFirstEmptyPage();
    if (--page < 0)
    {
        this->save(false);
        return;
    }

    printf("page is %d\r\n", page);

    int addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
    int *data = (int *)addr;

    if (data[0] == SETTINGS_KEY)
    {
        printf("channel %d\r\n", data[1]);
        this->channel = data[1];
    }
    else
    {
        this->save(false);
    }
}

int Settings::getFirstEmptyPage()
{
    int *p, page, addr;

    for (page = 0; page < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; page++)
    {
        addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
        p = (int *)addr;
        if (*p == -1)
        {
            return page;
        }
    }

    return -1;
}

void Settings::save(bool format)
{
    int buf[FLASH_PAGE_SIZE / sizeof(int)]; // One page buffer of ints

    int firstEmptyPage = this->getFirstEmptyPage();

    buf[0] = SETTINGS_KEY;
    buf[1] = channel;

    if (firstEmptyPage < 0 || format)
    {
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
        restore_interrupts(ints);
        firstEmptyPage = 0;
    }

    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(FLASH_TARGET_OFFSET + (firstEmptyPage * FLASH_PAGE_SIZE), (uint8_t *)buf, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}
