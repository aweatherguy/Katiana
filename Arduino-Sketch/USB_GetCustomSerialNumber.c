//
// returns the serial number obtained from Katiana flash, if present.
// reads from flash the first time called, thereafter uses value cached in SRAM.
//
uint8_t USB_GetCustomSerialNumber(uint8_t **Buffer)
{
    if (! Buffer) return 0;
    *Buffer = usbSerial;
    //
    // return cached value, if available. 
    //
    if (usbSerialCached) return usbSerialLength;    
    //
    // Copy the USB serial number from bootloader flash (if it exists).
    // It is bootloader flash as a simple ASCII null terminatede string.
    // We have to read it once to find the length, then malloc() the memory 
    // to hold it. Then read it again to copy it.
    //
    // Because we have not yet cached flash S/N information in SRAM,
    // the following variables will still have their initial values
    // of zero that were set in the sketch startup code:
    //
    //          usbSerialLength, usbSerial, usbSerialCached
    //
    usbSerialCached = 1;

    uint16_t snptr = pgm_read_word(FLASHEND-7);
    if (snptr == 0x0000u || snptr == 0xffffu) return 0;

    uint16_t p = snptr;
    uint8_t cnt = 0;
    while (cnt < 127)
    {
        if (pgm_read_byte(p++) == 0) break;
        cnt++;
    }
    if ((cnt < 12) || (cnt > 126)) return 0;
    //
    // serial number length is good, get some SRAM and copy it in there
    //
    if ( ! (usbSerial = (uint8_t *)malloc(cnt + 1) ) ) return 0;
    //
    // all is good so update the cached s/n length and copy it from flash
    //
    usbSerialLength = cnt;
    uint8_t k;
    uint8_t *s = usbSerial;

    for (k = usbSerialLength; k; k--)
    {
	*s++ = pgm_read_byte(snptr++);
    }
    *s = 0;     // terminate the string

    *Buffer = usbSerial;    // previous value stored in *Buffer was not valid
    return usbSerialLength;
}
