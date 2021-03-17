// include bosch header for the #defines.
#include <iostream>
#include <chrono>
#include <thread>

#include "./BMP280_driver/bmp280.h"
#include "bmp280.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
BMP280::BMP280(int pAddress,int pBus):Device(pAddress,pBus),
                mChipId(BMP280_INIT_VALUE),
                mTemp(0),
                mPressure(0)
{
    if( !IsOpen() )
    {
        std::cout << "Failed to open BMP280" << std::endl;
        return;
    }

    bool GotChipId = false;
	for( int n = 0 ; n < BMP280_CHIP_ID_READ_COUNT && GotChipId == false ; n++ )
    {
		/* read chip id */
        if( ReadData(BMP280_CHIP_ID_REG, &mChipId,BMP280_GEN_READ_WRITE_DATA_LENGTH) != -1 &&
          (mChipId == BMP280_CHIP_ID1 || mChipId == BMP280_CHIP_ID2 || mChipId == BMP280_CHIP_ID3))
        {// Done, make loop exit.
            GotChipId = true;
        }
        else
        {// Did not get chip id, try again.
            /* Delay added concerning the low speed of power up system to
            facilitate the proper reading of the chip ID */
            std::this_thread::sleep_for(std::chrono::milliseconds(BMP280_REGISTER_READ_DELAY));
        }
	}

    if( !GotChipId )
    {
        std::cout << "Failed to find BMP280 chip Id. Check chip address." << std::endl;
        Close();
        return;        
    }

	/* variable used to return communication result*/
	u8 a_data_u8[BMP280_CALIB_DATA_SIZE] = {BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE};

	if( ReadData(BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG,a_data_u8,BMP280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH) == -1 )
    {
        std::cout << "Failed to get BMP280 calibration parameters." << std::endl;
        return;        
    }

    /* read calibration values*/
    mCalibrationParameters.dig_T1 = (u16)((((u16)((u8)a_data_u8[
                BMP280_TEMPERATURE_CALIB_DIG_T1_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_TEMPERATURE_CALIB_DIG_T1_LSB]);
    mCalibrationParameters.dig_T2 = (s16)((((s16)((s8)a_data_u8[
                BMP280_TEMPERATURE_CALIB_DIG_T2_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_TEMPERATURE_CALIB_DIG_T2_LSB]);
    mCalibrationParameters.dig_T3 = (s16)((((s16)((s8)a_data_u8[
                BMP280_TEMPERATURE_CALIB_DIG_T3_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_TEMPERATURE_CALIB_DIG_T3_LSB]);
    mCalibrationParameters.dig_P1 = (u16)((((u16)((u8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P1_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P1_LSB]);
    mCalibrationParameters.dig_P2 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P2_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P2_LSB]);
    mCalibrationParameters.dig_P3 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P3_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P3_LSB]);
    mCalibrationParameters.dig_P4 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P4_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P4_LSB]);
    mCalibrationParameters.dig_P5 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P5_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P5_LSB]);
    mCalibrationParameters.dig_P6 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P6_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P6_LSB]);
    mCalibrationParameters.dig_P7 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P7_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P7_LSB]);
    mCalibrationParameters.dig_P8 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P8_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P8_LSB]);
    mCalibrationParameters.dig_P9 = (s16)((((s16)((s8)a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P9_MSB]))
                << BMP280_SHIFT_BIT_POSITION_BY_08_BITS)
                | a_data_u8[
                BMP280_PRESSURE_CALIB_DIG_P9_LSB]);
}

BMP280::~BMP280()
{

}

bool BMP280::Read()
{
    bool ReadOk = false;
    if(IsOpen() &&
        SetWorkModePowerMode(BMP280_ULTRA_LOW_POWER_MODE,BMP280_NORMAL_MODE) &&
        SetStandbyMode(BMP280_STANDBY_TIME_1_MS) )
    {
        int32_t rPressureRaw,rTemperatureRaw;
        if( ReadRawData(rPressureRaw,rTemperatureRaw) )
        {
            /* calculate x1*/
            float v_x1_u32r = (((float)rTemperatureRaw) / 16384.0f -
                ((float)mCalibrationParameters.dig_T1) / 1024.0f) *
                ((float)mCalibrationParameters.dig_T2);
            /* calculate x2*/
            float v_x2_u32r = ((((float)rTemperatureRaw) / 131072.0f -
                ((float)mCalibrationParameters.dig_T1) / 8192.0f) *
                (((float)rTemperatureRaw) / 131072.0f -
                ((float)mCalibrationParameters.dig_T1) / 8192.0f)) *
                ((float)mCalibrationParameters.dig_T3);

            /* calculate t_fine Used for pressure calculation*/
            mCalibrationParameters.t_fine = (s32)(v_x1_u32r + v_x2_u32r);

            /* calculate true temperature*/
            mTemp = (v_x1_u32r + v_x2_u32r) / 5120.0f;


            v_x1_u32r = ((float)mCalibrationParameters.t_fine/2.0) - 64000.0f;
            v_x2_u32r = v_x1_u32r * v_x1_u32r * ((float)mCalibrationParameters.dig_P6) / 32768.0f;
            v_x2_u32r = v_x2_u32r + v_x1_u32r * ((float)mCalibrationParameters.dig_P5) * 2.0f;
            v_x2_u32r = (v_x2_u32r / 4.0f) + (((float)mCalibrationParameters.dig_P4) * 65536.0f);
            v_x1_u32r = (((float)mCalibrationParameters.dig_P3) *  v_x1_u32r * v_x1_u32r / 524288.0f +
                ((float)mCalibrationParameters.dig_P2) * v_x1_u32r) / 524288.0f;
            v_x1_u32r = (1.0 + v_x1_u32r / 32768.0f) * ((float)mCalibrationParameters.dig_P1);
            mPressure = 1048576.0f - (float)rPressureRaw;
            /* Avoid exception caused by division by zero */
            if ((v_x1_u32r > 0) || (v_x1_u32r < 0))
            {
                mPressure = (mPressure - (v_x2_u32r / 4096.0f)) * 6250.0f / v_x1_u32r;
                v_x1_u32r = ((float)mCalibrationParameters.dig_P9) * mPressure * mPressure / 2147483648.0f;
                v_x2_u32r = mPressure * ((float)mCalibrationParameters.dig_P8) / 32768.0f;
                mPressure = mPressure + (v_x1_u32r + v_x2_u32r +  ((float)mCalibrationParameters.dig_P7)) / 16.0f;
                ReadOk = true;
            }
            else
            {
                mPressure = 0;
            }

        }

        // When done, go back to low lower mode as per the driver example.
        SetWorkModePowerMode(BMP280_ULTRA_LOW_POWER_MODE,BMP280_SLEEP_MODE);
    }
    return ReadOk;
}

bool BMP280::SetWorkModePowerMode(uint8_t pWorkMode,uint8_t pPowerMode)
{
    uint8_t mOversampleTemperature;
    uint8_t mOversamplePressure;
    
    if( !IsOpen() && pWorkMode <= BMP280_ULTRA_HIGH_RESOLUTION_MODE && pPowerMode <= BMP280_NORMAL_MODE)
        return false;

    switch (pWorkMode)
    {
    /* write work mode*/
    case BMP280_ULTRA_LOW_POWER_MODE:
        mOversampleTemperature = BMP280_ULTRALOWPOWER_OVERSAMP_TEMPERATURE;
        mOversamplePressure = BMP280_ULTRALOWPOWER_OVERSAMP_PRESSURE;
        break;

    case BMP280_LOW_POWER_MODE:
        mOversampleTemperature = BMP280_LOWPOWER_OVERSAMP_TEMPERATURE;
        mOversamplePressure = BMP280_LOWPOWER_OVERSAMP_PRESSURE;
        break;

    case BMP280_STANDARD_RESOLUTION_MODE:
        mOversampleTemperature = BMP280_STANDARDRESOLUTION_OVERSAMP_TEMPERATURE;
        mOversamplePressure = BMP280_STANDARDRESOLUTION_OVERSAMP_PRESSURE;
        break;

    case BMP280_HIGH_RESOLUTION_MODE:
        mOversampleTemperature = BMP280_HIGHRESOLUTION_OVERSAMP_TEMPERATURE;
        mOversamplePressure = BMP280_HIGHRESOLUTION_OVERSAMP_PRESSURE;
        break;

    case BMP280_ULTRA_HIGH_RESOLUTION_MODE:
        mOversampleTemperature = BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_TEMPERATURE;
        mOversamplePressure = BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_PRESSURE;
        break;
    }

    pPowerMode = BMP280_SET_BITSLICE(pPowerMode,BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE,mOversampleTemperature);
    pPowerMode = BMP280_SET_BITSLICE(pPowerMode,BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE, mOversamplePressure);

    return WriteData(BMP280_CTRL_MEAS_REG,&pPowerMode, BMP280_GEN_READ_WRITE_DATA_LENGTH) != -1;

}

bool BMP280::SetStandbyMode(uint8_t pMode)
{
	u8 v_data_u8 = BMP280_INIT_VALUE;

    /* write the standby duration*/
    if( IsOpen() && ReadData(BMP280_CONFIG_REG_STANDBY_DURN__REG,&v_data_u8,BMP280_GEN_READ_WRITE_DATA_LENGTH) != -1 )
    {
        v_data_u8 = BMP280_SET_BITSLICE(v_data_u8,BMP280_CONFIG_REG_STANDBY_DURN,pMode);
        return WriteData(BMP280_CONFIG_REG_STANDBY_DURN__REG,&pMode,BMP280_GEN_READ_WRITE_DATA_LENGTH) != -1;
	}

    return false;
}

bool BMP280::ReadRawData(int32_t& rPressureRaw, int32_t& rTemperatureRaw)
{
	/* variable used to return communication result*/
	BMP280_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	/* Array holding the temperature and pressure data
	 a_data_u8[0] - Pressure MSB
	 a_data_u8[1] - Pressure LSB
	 a_data_u8[2] - Pressure LSB
	 a_data_u8[3] - Temperature MSB
	 a_data_u8[4] - Temperature LSB
	 a_data_u8[5] - Temperature LSB
	 */
	u8 a_data_u8[BMP280_ALL_DATA_FRAME_LENGTH] = {BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE, BMP280_INIT_VALUE,
			BMP280_INIT_VALUE, BMP280_INIT_VALUE};
    if( IsOpen() )
    {
        if( ReadData(BMP280_PRESSURE_MSB_REG, a_data_u8, BMP280_DATA_FRAME_SIZE) )
        {
            /*Pressure*/
            rPressureRaw = (int32_t)((((uint32_t)(
                    a_data_u8[BMP280_DATA_FRAME_PRESSURE_MSB_BYTE]))
                    << BMP280_SHIFT_BIT_POSITION_BY_12_BITS)
                    | (((uint32_t)(
                    a_data_u8[BMP280_DATA_FRAME_PRESSURE_LSB_BYTE]))
                    << BMP280_SHIFT_BIT_POSITION_BY_04_BITS)
                    | ((uint32_t)a_data_u8[
                    BMP280_DATA_FRAME_PRESSURE_XLSB_BYTE]
                    >> BMP280_SHIFT_BIT_POSITION_BY_04_BITS));

            /* Temperature */
            rTemperatureRaw = (int32_t)((((uint32_t)(a_data_u8[
                    BMP280_DATA_FRAME_TEMPERATURE_MSB_BYTE]))
                    << BMP280_SHIFT_BIT_POSITION_BY_12_BITS)
                    | (((uint32_t)(a_data_u8[
                    BMP280_DATA_FRAME_TEMPERATURE_LSB_BYTE]))
                    << BMP280_SHIFT_BIT_POSITION_BY_04_BITS)
                    | ((uint32_t)a_data_u8[
                    BMP280_DATA_FRAME_TEMPERATURE_XLSB_BYTE]
                    >> BMP280_SHIFT_BIT_POSITION_BY_04_BITS));
            return true;
        }
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
};//namespace i2c{
