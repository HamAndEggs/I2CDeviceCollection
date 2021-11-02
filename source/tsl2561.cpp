#include <assert.h>
#include <iostream>

#include <thread>

#include "tsl2561.h"

using namespace std::chrono;

namespace i2c{
//////////////////////////////////////////////////////////////////////////
TSL2561::TSL2561(int pAddress,int pBus,bool pPackageCS):
	Device(pAddress,pBus),
	mPackageCS(pPackageCS),
	mIntergrationTime(INTERGRATION_TIME_401MS),
	mEnableGain(true),
	mIR(0),
	mLux(0),
	mNextRead(0),
	mReadingReady(0)
{

	if( IsOpen() )
	{
		WriteByteData(0x80,0x03);// Power on.
		int PartId = ReadByteData(0x0A);
		WriteByteData(0x80,0x00);// Power off.

		if( PartId == 0x0A )
		{
			UpdateRegisterTiming();
			Read(0);// Init first read.
		}
		else
		{
			std::cout << "i2c device at address 0x" << std::hex << pAddress << " is not an tsl2561. PartId: " << (int)PartId << std::dec << std::endl;
			Close();
			return;
		}
	}
}

TSL2561::~TSL2561()
{
}

void TSL2561::SetIntergrationTime(int pTime)
{
	assert( pTime == INTERGRATION_TIME_13MS || pTime == INTERGRATION_TIME_101MS || pTime == INTERGRATION_TIME_401MS );

	if( pTime == INTERGRATION_TIME_13MS || pTime == INTERGRATION_TIME_101MS || pTime == INTERGRATION_TIME_401MS )
	{
		mIntergrationTime = pTime;
	}
	UpdateRegisterTiming();
}

void TSL2561::SetEnableGain(bool pEnable)
{
	mEnableGain = pEnable;
	UpdateRegisterTiming();
}

void TSL2561::Read(int pMilliseconds)
{
//	if( IsOpen() )
	{
		milliseconds NOW = time_point_cast<milliseconds>(high_resolution_clock::now()).time_since_epoch();

		if( mReadingReady < NOW )
		{
			// Take reading.
			uint32_t channel0 = ReadWordData(0xAC);	// Read channel 0 which is Vis + IR.
			uint32_t channel1 = ReadWordData(0xAE);	// Read channel 1 which is IR.

			mLux = CalculateLux(channel0,channel1);
			mIR = channel1;

			WriteByteData(0x80,0x00);// Power off. Wait for when to start next reading.

			mReadingReady = milliseconds::max();// So don't trigger again. Set when next read is triggered.
		}
		else if( mNextRead < NOW )
		{
			WriteByteData(0x80,0x03);// Power on.

			// Set time to stat next read. And time to do the read, waiting for the intergration time.
			switch (mIntergrationTime)
			{
			case INTERGRATION_TIME_13MS: // 13.7 msec
				if( pMilliseconds < 14 )
					pMilliseconds = 14;
				mNextRead = NOW + milliseconds(pMilliseconds);
				mReadingReady = NOW + milliseconds(14);
				break;

			case INTERGRATION_TIME_101MS: // 101 msec
				if( pMilliseconds < 101 )
					pMilliseconds = 101;
				mNextRead = NOW + milliseconds(pMilliseconds);
				mReadingReady = NOW + milliseconds(101);
				break;

			default: // assume no scaling
			case INTERGRATION_TIME_401MS:
				if( pMilliseconds < 401 )
					pMilliseconds = 401;
				mNextRead = NOW + milliseconds(pMilliseconds);
				mReadingReady = NOW + milliseconds(401);
				break;
			}
		}
	}
}


// Code taken from datasheet.
//**************************************************************************** //
// Copyright © ams AG, Inc.
//
// THIS CODE AND INFORMATION IS PROVIDED ”AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
//****************************************************************************
#define LUX_SCALE 14 // scale by 2^14
#define RATIO_SCALE 9 // scale ratio by 2^9
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
// Integration time scaling factors
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
#define CH_SCALE 10 // scale channel values by 2^10
#define CHSCALE_TINT0 0x7517 // 322/11 * 2^CH_SCALE
#define CHSCALE_TINT1 0x0fe7 // 322/81 * 2^CH_SCALE
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
// T, FN, and CL Package coefficients
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
// For Ch1/Ch0=0.00 to 0.50
// Lux/Ch0=0.0304−0.062*((Ch1/Ch0)^1.4)
// piecewise approximation
// For Ch1/Ch0=0.00 to 0.125:
// Lux/Ch0=0.0304−0.0272*(Ch1/Ch0)
//
// For Ch1/Ch0=0.125 to 0.250:
// Lux/Ch0=0.0325−0.0440*(Ch1/Ch0)
//
// For Ch1/Ch0=0.250 to 0.375:
// Lux/Ch0=0.0351−0.0544*(Ch1/Ch0)
//
// For Ch1/Ch0=0.375 to 0.50:
// Lux/Ch0=0.0381−0.0624*(Ch1/Ch0)
//
// For Ch1/Ch0=0.50 to 0.61:
// Lux/Ch0=0.0224−0.031*(Ch1/Ch0)
//
// For Ch1/Ch0=0.61 to 0.80:
// Lux/Ch0=0.0128−0.0153*(Ch1/Ch0)
//
// For Ch1/Ch0=0.80 to 1.30:
// Lux/Ch0=0.00146−0.00112*(Ch1/Ch0)
//
// For Ch1/Ch0>1.3:
// Lux/Ch0=0
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
#define K1T 0x0040 // 0.125 * 2^RATIO_SCALE
#define B1T 0x01f2 // 0.0304 * 2^LUX_SCALE
#define M1T 0x01be // 0.0272 * 2^LUX_SCALE
#define K2T 0x0080 // 0.250 * 2^RATIO_SCALE
#define B2T 0x0214 // 0.0325 * 2^LUX_SCALE
#define M2T 0x02d1 // 0.0440 * 2^LUX_SCALE
#define K3T 0x00c0 // 0.375 * 2^RATIO_SCALE
#define B3T 0x023f // 0.0351 * 2^LUX_SCALE
#define M3T 0x037b // 0.0544 * 2^LUX_SCALE
#define K4T 0x0100 // 0.50 * 2^RATIO_SCALE
#define B4T 0x0270 // 0.0381 * 2^LUX_SCALE
#define M4T 0x03fe // 0.0624 * 2^LUX_SCALE
#define K5T 0x0138 // 0.61 * 2^RATIO_SCALE
#define B5T 0x016f // 0.0224 * 2^LUX_SCALE
#define M5T 0x01fc // 0.0310 * 2^LUX_SCALE
#define K6T 0x019a // 0.80 * 2^RATIO_SCALE
#define B6T 0x00d2 // 0.0128 * 2^LUX_SCALE
#define M6T 0x00fb // 0.0153 * 2^LUX_SCALE
#define K7T 0x029a // 1.3 * 2^RATIO_SCALE
#define B7T 0x0018 // 0.00146 * 2^LUX_SCALE
#define M7T 0x0012 // 0.00112 * 2^LUX_SCALE
#define K8T 0x029a // 1.3 * 2^RATIO_SCALE
#define B8T 0x0000 // 0.000 * 2^LUX_SCALE
#define M8T 0x0000 // 0.000 * 2^LUX_SCALE
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
// CS package coefficients
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
// For 0 <= Ch1/Ch0 <= 0.52
// Lux/Ch0 = 0.0315−0.0593*((Ch1/Ch0)^1.4)
// piecewise approximation
// For 0 <= Ch1/Ch0 <= 0.13
// Lux/Ch0 = 0.0315−0.0262*(Ch1/Ch0)
// For 0.13 <= Ch1/Ch0 <= 0.26
// Lux/Ch0 = 0.0337−0.0430*(Ch1/Ch0)
// For 0.26 <= Ch1/Ch0 <= 0.39
// Lux/Ch0 = 0.0363−0.0529*(Ch1/Ch0)
// For 0.39 <= Ch1/Ch0 <= 0.52
// Lux/Ch0 = 0.0392−0.0605*(Ch1/Ch0)
// For 0.52 < Ch1/Ch0 <= 0.65
// Lux/Ch0 = 0.0229−0.0291*(Ch1/Ch0)
// For 0.65 < Ch1/Ch0 <= 0.80
// Lux/Ch0 = 0.00157−0.00180*(Ch1/Ch0)
// For 0.80 < Ch1/Ch0 <= 1.30
// Lux/Ch0 = 0.00338−0.00260*(Ch1/Ch0)
// For Ch1/Ch0 > 1.30
// Lux = 0
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
#define K1C 0x0043 // 0.130 * 2^RATIO_SCALE
#define B1C 0x0204 // 0.0315 * 2^LUX_SCALE
#define M1C 0x01ad // 0.0262 * 2^LUX_SCALE
#define K2C 0x0085 // 0.260 * 2^RATIO_SCALE
#define B2C 0x0228 // 0.0337 * 2^LUX_SCALE
#define M2C 0x02c1 // 0.0430 * 2^LUX_SCALE
#define K3C 0x00c8 // 0.390 * 2^RATIO_SCALE
#define B3C 0x0253 // 0.0363 * 2^LUX_SCALE
#define M3C 0x0363 // 0.0529 * 2^LUX_SCALE
#define K4C 0x010a // 0.520 * 2^RATIO_SCALE
#define B4C 0x0282 // 0.0392 * 2^LUX_SCALE
#define M4C 0x03df // 0.0605 * 2^LUX_SCALE
#define K5C 0x014d // 0.65 * 2^RATIO_SCALE
#define B5C 0x0177 // 0.0229 * 2^LUX_SCALE
#define M5C 0x01dd // 0.0291 * 2^LUX_SCALE
#define K6C 0x019a // 0.80 * 2^RATIO_SCALE
#define B6C 0x0101 // 0.0157 * 2^LUX_SCALE
#define M6C 0x0127 // 0.0180 * 2^LUX_SCALE
#define K7C 0x029a // 1.3 * 2^RATIO_SCALE
#define B7C 0x0037 // 0.00338 * 2^LUX_SCALE
#define M7C 0x002b // 0.00260 * 2^LUX_SCALE
#define K8C 0x029a // 1.3 * 2^RATIO_SCALE
#define B8C 0x0000 // 0.000 * 2^LUX_SCALE
#define M8C 0x0000 // 0.000 * 2^LUX_SCALE
// lux equation approximation without floating point calculations
//////////////////////////////////////////////////////////////////////////////
// Routine: unsigned int CalculateLux(unsigned int ch0, unsigned int ch0, int iType)
//
// Description: Calculate the approximate illuminance (lux) given the raw
// channel values of the TSL2560. The equation if implemented
// as a piece−wise linear approximation.
//
// Arguments: unsigned int iGain − gain, where 0:1X, 1:16X
// unsigned int tInt − integration time, where 0:13.7mS, 1:100mS, 2:402mS,
// 3:Manual
// unsigned int ch0 − raw channel value from channel 0 of TSL2560
// unsigned int ch1 − raw channel value from channel 1 of TSL2560
// unsigned int iType − package type (T or CS)
//
// Return: unsigned int − the approximate illuminance (lux)
//
//////////////////////////////////////////////////////////////////////////////
//static unsigned int CalculateLux(unsigned int iGain, unsigned int tInt, unsigned int ch0,unsigned int ch1, int iType)
int TSL2561::CalculateLux(uint32_t pChannel0,uint32_t pChannel1)const
{
	//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
	// first, scale the channel values depending on the gain and integration time
	// 16X, 402mS is nominal.
	// scale if integration time is NOT 402 msec
	uint32_t chScale;
	switch (mIntergrationTime)
	{
	case INTERGRATION_TIME_13MS: // 13.7 msec
		chScale = CHSCALE_TINT0;
		break;

	case INTERGRATION_TIME_101MS: // 101 msec
		chScale = CHSCALE_TINT1;
		break;

	default: // assume no scaling
	case INTERGRATION_TIME_401MS:
		chScale = (1 << CH_SCALE);
		break;
	}

	// scale if gain is NOT 16X
	if(mEnableGain == false)
	{
		chScale = chScale << 4;
	} // scale 1X to 16X

	// scale the channel values
	uint32_t channel0 = (pChannel0 * chScale) >> CH_SCALE;
	uint32_t channel1 = (pChannel1 * chScale) >> CH_SCALE;

	//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
	// find the ratio of the channel values (Channel1/Channel0)
	// protect against divide by zero
	uint32_t ratio1 = 0;
	if (channel0 != 0)
	{
		ratio1 = (channel1 << (RATIO_SCALE+1)) / channel0;
	}

	// round the ratio value
	uint32_t ratio = (ratio1 + 1) >> 1;
	// is ratio <= eachBreak ?
	uint32_t b, m;
	if(mPackageCS)
	{// CS package
		if ((ratio >= 0) && (ratio <= K1C))
		{b=B1C; m=M1C;}
		else if (ratio <= K2C)
		{b=B2C; m=M2C;}
		else if (ratio <= K3C)
		{b=B3C; m=M3C;}
		else if (ratio <= K4C)
		{b=B4C; m=M4C;}
		else if (ratio <= K5C)
		{b=B5C; m=M5C;}
		else if (ratio <= K6C)
		{b=B6C; m=M6C;}
		else if (ratio <= K7C)
		{b=B7C; m=M7C;}
		else if (ratio > K8C)
		{b=B8C; m=M8C;}
	}
	else
	{// T, FN and CL package  (the one adafruit uses)
		if ((ratio >= 0) && (ratio <= K1T))
		{b=B1T; m=M1T;}
		else if (ratio <= K2T)
		{b=B2T; m=M2T;}
		else if (ratio <= K3T)
		{b=B3T; m=M3T;}
		else if (ratio <= K4T)
		{b=B4T; m=M4T;}
		else if (ratio <= K5T)
		{b=B5T; m=M5T;}
		else if (ratio <= K6T)
		{b=B6T; m=M6T;}
		else if (ratio <= K7T)
		{b=B7T; m=M7T;}
		else if (ratio > K8T)
		{b=B8T; m=M8T;}
	}

	uint32_t temp = ((channel0 * b) - (channel1 * m));
	// do not allow negative lux value
	if (temp < 0)
	{
		temp = 0;
	}

	// round lsb (2^(LUX_SCALE−1))
	temp += (1 << (LUX_SCALE-1));

	// strip off fractional portion
	uint32_t lux = temp >> LUX_SCALE;

	return (int)lux;
}

void TSL2561::UpdateRegisterTiming()
{
	assert( IsOpen() );
	uint8_t value = 0;
	if( mEnableGain )
		value = 0x10;

	switch(mIntergrationTime)
	{
	case INTERGRATION_TIME_13MS:
		value |= 0x00;
		break;		

	case INTERGRATION_TIME_101MS:
		value |= 0x01;
		break;		

	default:
	case INTERGRATION_TIME_401MS:
		value |= 0x02;
		break;		
	}

	WriteByteData(0x81,value);
}

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{
