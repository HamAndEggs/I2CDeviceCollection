#include <iostream>
#include <chrono>
#include <thread>

#include "si1145.h"

/* COMMANDS */
#define SI1145_PARAM_QUERY 0x80
#define SI1145_PARAM_SET 0xA0
#define SI1145_NOP 0x0
#define SI1145_RESET    0x01
#define SI1145_BUSADDR    0x02
#define SI1145_PS_FORCE    0x05
#define SI1145_ALS_FORCE    0x06
#define SI1145_PSALS_FORCE    0x07
#define SI1145_PS_PAUSE    0x09
#define SI1145_ALS_PAUSE    0x0A
#define SI1145_PSALS_PAUSE    0xB
#define SI1145_PS_AUTO    0x0D
#define SI1145_ALS_AUTO   0x0E
#define SI1145_PSALS_AUTO 0x0F
#define SI1145_GET_CAL    0x12

/* Parameters */
#define SI1145_PARAM_I2CADDR 0x00
#define SI1145_PARAM_CHLIST   0x01
#define SI1145_PARAM_CHLIST_ENUV 0x80
#define SI1145_PARAM_CHLIST_ENAUX 0x40
#define SI1145_PARAM_CHLIST_ENALSIR 0x20
#define SI1145_PARAM_CHLIST_ENALSVIS 0x10
#define SI1145_PARAM_CHLIST_ENPS1 0x01
#define SI1145_PARAM_CHLIST_ENPS2 0x02
#define SI1145_PARAM_CHLIST_ENPS3 0x04

#define SI1145_PARAM_PSLED12SEL   0x02
#define SI1145_PARAM_PSLED12SEL_PS2NONE 0x00
#define SI1145_PARAM_PSLED12SEL_PS2LED1 0x10
#define SI1145_PARAM_PSLED12SEL_PS2LED2 0x20
#define SI1145_PARAM_PSLED12SEL_PS2LED3 0x40
#define SI1145_PARAM_PSLED12SEL_PS1NONE 0x00
#define SI1145_PARAM_PSLED12SEL_PS1LED1 0x01
#define SI1145_PARAM_PSLED12SEL_PS1LED2 0x02
#define SI1145_PARAM_PSLED12SEL_PS1LED3 0x04

#define SI1145_PARAM_PSLED3SEL   0x03
#define SI1145_PARAM_PSENCODE   0x05
#define SI1145_PARAM_ALSENCODE  0x06

#define SI1145_PARAM_PS1ADCMUX   0x07
#define SI1145_PARAM_PS2ADCMUX   0x08
#define SI1145_PARAM_PS3ADCMUX   0x09
#define SI1145_PARAM_PSADCOUNTER   0x0A
#define SI1145_PARAM_PSADCGAIN 0x0B
#define SI1145_PARAM_PSADCMISC 0x0C
#define SI1145_PARAM_PSADCMISC_RANGE 0x20
#define SI1145_PARAM_PSADCMISC_PSMODE 0x04

#define SI1145_PARAM_ALSIRADCMUX   0x0E
#define SI1145_PARAM_AUXADCMUX   0x0F

#define SI1145_PARAM_ALSVISADCOUNTER   0x10
#define SI1145_PARAM_ALSVISADCGAIN 0x11
#define SI1145_PARAM_ALSVISADCMISC 0x12
#define SI1145_PARAM_ALSVISADCMISC_VISRANGE 0x20

#define SI1145_PARAM_ALSIRADCOUNTER   0x1D
#define SI1145_PARAM_ALSIRADCGAIN 0x1E
#define SI1145_PARAM_ALSIRADCMISC 0x1F
#define SI1145_PARAM_ALSIRADCMISC_RANGE 0x20

#define SI1145_PARAM_ADCCOUNTER_511CLK 0x70

#define SI1145_PARAM_ADCMUX_SMALLIR  0x00
#define SI1145_PARAM_ADCMUX_LARGEIR  0x03



/* REGISTERS */
#define SI1145_REG_PARTID  0x00
#define SI1145_REG_REVID  0x01
#define SI1145_REG_SEQID  0x02

#define SI1145_REG_INTCFG  0x03
#define SI1145_REG_INTCFG_INTOE 0x01
#define SI1145_REG_INTCFG_INTMODE 0x02

#define SI1145_REG_IRQEN  0x04
#define SI1145_REG_IRQEN_ALSEVERYSAMPLE 0x01
#define SI1145_REG_IRQEN_PS1EVERYSAMPLE 0x04
#define SI1145_REG_IRQEN_PS2EVERYSAMPLE 0x08
#define SI1145_REG_IRQEN_PS3EVERYSAMPLE 0x10

#include <chrono>
#define SI1145_REG_IRQMODE1 0x05
#define SI1145_REG_IRQMODE2 0x06

#define SI1145_REG_HWKEY  0x07
#define SI1145_REG_MEASRATE0 0x08
#define SI1145_REG_MEASRATE1  0x09
#define SI1145_REG_PSRATE  0x0A
#define SI1145_REG_PSLED21  0x0F
#define SI1145_REG_PSLED3  0x10
#define SI1145_REG_UCOEFF0  0x13
#define SI1145_REG_UCOEFF1  0x14
#define SI1145_REG_UCOEFF2  0x15
#define SI1145_REG_UCOEFF3  0x16
#define SI1145_REG_PARAMWR  0x17
#define SI1145_REG_COMMAND  0x18
#define SI1145_REG_RESPONSE  0x20
#define SI1145_REG_IRQSTAT  0x21
#define SI1145_REG_IRQSTAT_ALS  0x01

#define SI1145_REG_ALSVISDATA0 0x22
#define SI1145_REG_ALSVISDATA1 0x23
#define SI1145_REG_ALSIRDATA0 0x24
#define SI1145_REG_ALSIRDATA1 0x25
#define SI1145_REG_PS1DATA0 0x26
#define SI1145_REG_PS1DATA1 0x27
#define SI1145_REG_PS2DATA0 0x28
#define SI1145_REG_PS2DATA1 0x29
#define SI1145_REG_PS3DATA0 0x2A
#define SI1145_REG_PS3DATA1 0x2B
#define SI1145_REG_UVINDEX0 0x2C
#define SI1145_REG_UVINDEX1 0x2D
#define SI1145_REG_PARAMRD 0x2E
#define SI1145_REG_CHIPSTAT 0x30

namespace i2c{
//////////////////////////////////////////////////////////////////////////
SI1145::SI1145(int pAddress,int pBus):Device(pAddress,pBus)
{
	uint8_t PartId;
	ReadData(SI1145_REG_PARTID,&PartId,1);
	if( PartId != 0x45 )
	{
		std::cout << "i2c device at address 0x" << std::hex << pAddress << " is not an si1145. PartId: " << (int)PartId << std::dec << std::endl;
		Close();
		return;
	}

	std::cout << "SI1145 PartID=" << std::hex << (int)PartId << std::dec << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	WriteByteData(SI1145_REG_MEASRATE0, 0);
	WriteByteData(SI1145_REG_MEASRATE1, 0);
	WriteByteData(SI1145_REG_IRQEN, 0);
	WriteByteData(SI1145_REG_IRQMODE1, 0);
	WriteByteData(SI1145_REG_IRQMODE2, 0);
	WriteByteData(SI1145_REG_INTCFG, 0);
	WriteByteData(SI1145_REG_IRQSTAT, 0xFF);

	WriteByteData(SI1145_REG_COMMAND, SI1145_RESET);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	WriteByteData(SI1145_REG_HWKEY, 0x17);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	/***********************************/
	// enable UVindex measurement coefficients!
	WriteByteData(SI1145_REG_UCOEFF0, 0x29);
	WriteByteData(SI1145_REG_UCOEFF1, 0x89);
	WriteByteData(SI1145_REG_UCOEFF2, 0x02);
	WriteByteData(SI1145_REG_UCOEFF3, 0x00);

	// enable UV sensor
	WriteParam(SI1145_PARAM_CHLIST, SI1145_PARAM_CHLIST_ENUV |SI1145_PARAM_CHLIST_ENALSIR | SI1145_PARAM_CHLIST_ENALSVIS |SI1145_PARAM_CHLIST_ENPS1);

	// enable interrupt on every sample
	WriteByteData(SI1145_REG_INTCFG, SI1145_REG_INTCFG_INTOE);  
	WriteByteData(SI1145_REG_IRQEN, SI1145_REG_IRQEN_ALSEVERYSAMPLE);  

	/****************************** Prox Sense 1 */

	// program LED current
	WriteByteData(SI1145_REG_PSLED21, 0x03); // 20mA for LED 1 only

	WriteParam(SI1145_PARAM_PS1ADCMUX, SI1145_PARAM_ADCMUX_LARGEIR);
	// prox sensor #1 uses LED #1
	WriteParam(SI1145_PARAM_PSLED12SEL, SI1145_PARAM_PSLED12SEL_PS1LED1);
	// fastest clocks, clock div 1
	WriteParam(SI1145_PARAM_PSADCGAIN, 0);
	// take 511 clocks to measure
	WriteParam(SI1145_PARAM_PSADCOUNTER, SI1145_PARAM_ADCCOUNTER_511CLK);
	// in prox mode, high range
	WriteParam(SI1145_PARAM_PSADCMISC, SI1145_PARAM_PSADCMISC_RANGE|SI1145_PARAM_PSADCMISC_PSMODE);

	WriteParam(SI1145_PARAM_ALSIRADCMUX, SI1145_PARAM_ADCMUX_SMALLIR);  
	// fastest clocks, clock div 1
	WriteParam(SI1145_PARAM_ALSIRADCGAIN, 0);
	// take 511 clocks to measure
	WriteParam(SI1145_PARAM_ALSIRADCOUNTER, SI1145_PARAM_ADCCOUNTER_511CLK);
	// in high range mode
	WriteParam(SI1145_PARAM_ALSIRADCMISC, SI1145_PARAM_ALSIRADCMISC_RANGE);

	// fastest clocks, clock div 1
	WriteParam(SI1145_PARAM_ALSVISADCGAIN, 0);
	// take 511 clocks to measure
	WriteParam(SI1145_PARAM_ALSVISADCOUNTER, SI1145_PARAM_ADCCOUNTER_511CLK);
	// in high range mode (not normal signal)
	WriteParam(SI1145_PARAM_ALSVISADCMISC, SI1145_PARAM_ALSVISADCMISC_VISRANGE);

	/************************/

	// measurement rate for auto
	WriteByteData(SI1145_REG_MEASRATE0, 0xFF); // 255 * 31.25uS = 8ms

	// auto run
	WriteByteData(SI1145_REG_COMMAND, SI1145_PSALS_AUTO);
}

SI1145::~SI1145()
{
}


int SI1145::ReadVisible()const
{
	if( IsOpen() )
		return ReadWordData(0x22);
	return -1;
}

int SI1145::ReadIR()const
{
	if( IsOpen() )
		return ReadWordData(0x24);
	return -1;
}

int SI1145::ReadProx()const
{
	if( IsOpen() )
		return ReadWordData(0x26);
	return -1;
}

int SI1145::ReadUV()const
{
	if( IsOpen() )
		return ReadWordData(0x2C);
	return -1;
}

uint8_t SI1145::WriteParam(uint8_t p, uint8_t v)
{
	if( IsOpen() )
	{
		WriteByteData(SI1145_REG_PARAMWR, v);
		WriteByteData(SI1145_REG_COMMAND, p | SI1145_PARAM_SET);
		return ReadByteData(SI1145_REG_PARAMRD);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{
