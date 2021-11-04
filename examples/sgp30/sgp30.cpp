
#include <iostream>
#include <signal.h>

#include "sgp30.h"

static bool CTRL_C_Pressed = false;
static void CtrlHandler(int SigNum)
{
	static int numTimesAskedToExit = 0;
	if( numTimesAskedToExit > 2 )
	{
		std::cerr << "Asked to quit to many times, forcing exit in bad way\n";
		exit(1);
	}
    numTimesAskedToExit++;

	CTRL_C_Pressed = true;
}


int main(int argc, char *argv[])
{
// Display the constants defined by app build. \n";
    std::cout << "Application Version " << APP_VERSION << '\n';
    std::cout << "Build date and time " << APP_BUILD_DATE_TIME << '\n';
    std::cout << "Build date " << APP_BUILD_DATE << '\n';
    std::cout << "Build time " << APP_BUILD_TIME << '\n';

	signal(SIGINT,CtrlHandler);

    i2c::SGP30 airQuality;

    auto callback = [](int pReadingResult,uint16_t pECO2,uint16_t pTVOC)
    {
        if( pReadingResult == i2c::SGP30::READING_RESULT_VALID )
        {
            std::clog << "eCO2:" << pECO2 << " tVOC:" << pTVOC << "\n";
        }
        else if( pReadingResult == i2c::SGP30::READING_RESULT_FAILED )
        {
            std::clog << "Reading failed, quitting...\n";
            CTRL_C_Pressed = true;// kill main loop.
        }
    };

    if( airQuality.Start(callback) )
    {
        std::clog << "warming up...";
        // And wait for quit...
        // Normally we'll be doing other stuff...
        while( CTRL_C_Pressed == false )
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    airQuality.Stop();

// And quit";
    return EXIT_SUCCESS;
}
