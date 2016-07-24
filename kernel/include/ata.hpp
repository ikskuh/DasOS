#pragma once

#include <stdint.h>

enum class ATAError
{
	Success,
	InvalidBlockCount,
	NotInitialized,
	InitializationFailed,
	GenericError,
	FloatingDevice,
};

/**
 * @brief ATA device driver
 */
class ATADevice
{
private:
	bool master;
	struct { 
		uint16_t data, error, sectors, lbaLow, lbaMid, lbaHigh, devSelect, status, cmd, status1, control;
	} ports;
	int32_t sectorCount;
public:
	ATADevice(uint16_t basePort, bool master);
	
	/**
	 * @brief Returns the ATA status code.
	 */
	uint8_t status();
	
	/**
	 * @brief Gets the number of sectors of this device.
	 */
	ATAError getSectorCount(uint32_t *count);
	
	/**
	 * @brief Checks if the ATADevice is actually a floating bus.
	 *
	 * This checks if the device is reachable or not.
	 */
	bool isFloating();
	
	/**
	 * @brief Initializes the ata device
	 */
	ATAError initialize();
	
	/**
	 * @brief Reads a given number of sectors from the device.
	 * @param lba The linear block address.
	 * @param num The number of blocks on the device.
	 * @return    Error code
	 */
	ATAError read(void *buffer, uint32_t lba, uint32_t num);
};