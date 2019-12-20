/*
 * MCP7940.h
 *
 * Created: 22-9-2017 08:21:21
 *  Author: adri
 */ 


#ifndef MCP7940_H_
#define MCP7940_H_

	//#define MCP7940_SLAVE_ADDRESS	0xDE
	#define MCP7940_SLAVE_ADDRESS	0x6F
	/******************************************************************************************
											ENUMERATION
	******************************************************************************************/
	typedef enum
	{
		SEC_MASK	= 0x70,
		MIN_MASK	= 0x70,
		HOUR_MASK	= 0x10,
		WEEKDAY_MASK= 0x07,
		DATE_MASK	= 0x30,
		MONTH_MASK	= 0x10,
		YEAR_MASK	= 0xF0	
	}eMASKS;
	
	typedef enum
	{
		// Timekeeping
		RTC_SEC			= 0x00,
		RTC_MIN			= 0x01,
		RTC_HOUR		= 0x02,
		RTC_WEEKDAY		= 0x03,
		RTC_DATE		= 0x04,
		RTC_MONTH		= 0x05,
		RTC_YEAR		= 0x06,
		CONTROL			= 0x07,
		OSCTRIM			= 0x08,
		
		// Alarms 0
		ALARM0_SEC		= 0x0A,
		ALARM0_MIN		= 0x0B,
		ALARM0_HOUR		= 0x0C,	
		ALARM0_WEEKDAY	= 0x0D,
		ALARM0_DATE		= 0x0E,
		ALARM0_MONTH	= 0x0F,

		
		// Alarms 1
		ALARM1_SEC		= 0x11,
		ALARM1_MIN		= 0x12,
		ALARM1_HOUR		= 0x13,
		ALARM1_WEEKDAY	= 0x14,
		ALARM1_DATE		= 0x15,
		ALARM1_MONTH	= 0x16	
	}eMCP7940_REGISTERS;
	
	/******************************************************************************************
										Structures
	******************************************************************************************/
	typedef struct
	{
		uint8_t seconds;
		uint8_t	minutes;
		uint8_t hours;
		uint8_t	day;
		uint8_t	month;
		uint8_t year;
		uint8_t weekDay;
	}sTimeDate;



	/******************************************************************************************
										FUNCTION PROTOTYPES
	******************************************************************************************/
	void	mcp7940_init				(sTimeDate *TimeDate, uint8_t alarm_in_x_minutes);
	void	mcp7940_disable				(void);
	void	mcp7940_reset_minute_alarm	(uint8_t alarm_in_x_minutes);
	void	mcp7940_read_time_and_date	(sTimeDate *TimeDate);
	void	mcp7940_set_time_and_date	(sTimeDate *TimeDate);
	void	mcp7940_print				(sTimeDate *TimeDate);

#endif /* MCP7940_H_ */
