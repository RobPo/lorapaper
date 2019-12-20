/*
 * Cayenne_LPP.h
 *
 * Created: 4-10-2017 11:45:55
 *  Author: adri
 */ 


#ifndef CAYENNE_LPP_H_
#define CAYENNE_LPP_H_

	/******************************************************************************************
									INCLUDE FILES
	******************************************************************************************/

    #include "lorawan_def.h"

	/******************************************************************************************
									DEFINITIONS
	******************************************************************************************/

    #define LPP_DIGITAL_INPUT       0       // 1 byte
    #define LPP_DIGITAL_OUTPUT      1       // 1 byte
    #define LPP_ANALOG_INPUT        2       // 2 bytes, 0.01 signed
    #define LPP_ANALOG_OUTPUT       3       // 2 bytes, 0.01 signed
    #define LPP_LUMINOSITY          101     // 2 bytes, 1 lux unsigned
    #define LPP_PRESENCE            102     // 1 byte, 1
    #define LPP_TEMPERATURE         103     // 2 bytes, 0.1�C signed
    #define LPP_RELATIVE_HUMIDITY   104     // 1 byte, 0.5% unsigned
    #define LPP_ACCELEROMETER       113     // 2 bytes per axis, 0.001G
    #define LPP_BAROMETRIC_PRESSURE 115     // 2 bytes 0.1 hPa Unsigned
    #define LPP_GYROMETER           134     // 2 bytes per axis, 0.01 �/s
    #define LPP_GPS                 136     // 3 byte lon/lat 0.0001 �, 3 bytes alt 0.01 meter


    // Data ID + Data Type + Data Size
    #define LPP_DIGITAL_INPUT_SIZE       3       // 1 byte
    #define LPP_DIGITAL_OUTPUT_SIZE      3       // 1 byte
    #define LPP_ANALOG_INPUT_SIZE        4       // 2 bytes, 0.01 signed
    #define LPP_ANALOG_OUTPUT_SIZE       4       // 2 bytes, 0.01 signed
    #define LPP_LUMINOSITY_SIZE          4       // 2 bytes, 1 lux unsigned
    #define LPP_PRESENCE_SIZE            3       // 1 byte, 1
    #define LPP_TEMPERATURE_SIZE         4       // 2 bytes, 0.1�C signed
    #define LPP_RELATIVE_HUMIDITY_SIZE   3       // 1 byte, 0.5% unsigned
    #define LPP_ACCELEROMETER_SIZE       8       // 2 bytes per axis, 0.001G
    #define LPP_BAROMETRIC_PRESSURE_SIZE 4       // 2 bytes 0.1 hPa Unsigned
    #define LPP_GYROMETER_SIZE           8       // 2 bytes per axis, 0.01 �/s
    #define LPP_GPS_SIZE                 11      // 3 byte lon/lat 0.0001 �, 3 bytes alt 0.01 meter

	/******************************************************************************************
									FUNCTION PROTOTYPES
	******************************************************************************************/

	class CayenneLPP
	{
		public:
			CayenneLPP						(sLoRa_Message *buffer_Ptr);
			~CayenneLPP						();

			void	clearBuffer				(void);
			void	addGPS					(uint8_t channel, double latitude, double longitude, double altitude);
			void	addAnalogOutput			(uint8_t channel, double value);
			void	addDigitalOutput		(uint8_t channel, uint8_t value);
			void	addDigitalInput			(uint8_t channel, uint8_t value);
			void	addAnalogInput			(uint8_t channel, float value);
			void	addLuminosity			(uint8_t channel, float lux);
			void	addPresence				(uint8_t channel, uint8_t value);
			void	addTemperature			(uint8_t channel, float celsius);
			void	addRelativeHumidity		(uint8_t channel, float rh);
			void	addAccelerometer		(uint8_t channel, float x, float y, float z);
			void	addBarometricPressure	(uint8_t channel, float hpa);
			void	addGyrometer			(uint8_t channel, float x, float y, float z);
			
		private:
			sLoRa_Message *buffer;
	};

#endif /* CAYENNE_LPP_H_ */
