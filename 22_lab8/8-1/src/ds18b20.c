#include "ds18b20.h"

/* Send ConvT through OneWire with resolution
 * param:
 *   OneWire: send through this
 *   resolution: temperature resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
//tell temp start to convert and store it in scratch pad
int DS18B20_ConvT(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	DS18B20_SetResolution(OneWire, resolution);
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0x44);
	return 0;
}

/* Read temperature from OneWire
 * param:
 *   OneWire: send through this
 *   destination: output temperature
 * retval:
 *    0 -> OK
 *    1 -> Error
 *
 *
 *
 */
//tell temp to send back the data in scratch pad
uint8_t DS18B20_Read(OneWire_t* OneWire, float *destination) {
	int data;
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0xBE);
	data = OneWire_ReadByte(OneWire);
	data |= OneWire_ReadByte(OneWire) << 8;
	*destination = data / 16.0f;
	return 0;
}

/* Set resolution of the DS18B20
 * param:
 *   OneWire: send through this
 *   resolution: set to this resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
//set the temp bit
uint8_t DS18B20_SetResolution(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {

	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0x4E); // Command
	OneWire_WriteByte(OneWire, 0x00); // Threshold High
	OneWire_WriteByte(OneWire, 0x00); // Threshold Low
	OneWire_WriteByte(OneWire,  ((resolution - 9) << 5)| 0x1f); // Resolution (Configuration byte)

	return 0;
}

/* Check if the temperature conversion is done or not
 * param:
 *   OneWire: send through this
 * retval:
 *    0 -> OK
 *    1 -> Not yet
 */
//check that it weather convert the temp
uint8_t DS18B20_Done(OneWire_t* OneWire) {
	return !OneWire_ReadBit(OneWire);
}
