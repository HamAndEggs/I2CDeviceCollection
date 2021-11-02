
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
    uint16_t currentECO2 = 0;
    uint16_t currentTVOC = 0;

    auto callback = [&currentECO2,&currentTVOC](uint16_t pECO2,uint16_t pTVOC)
    {
        currentECO2 = pECO2;
        currentTVOC = pTVOC;
    };

    if( airQuality.Start(callback) )
    {
        // And wait for quit...
        // Normally we'll be doing other stuff...
        while( CTRL_C_Pressed == false )
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::clog << "eCO2:" << currentECO2 << " tVOC:" << currentTVOC << "\n";
        }
    }

    airQuality.Stop();

// And quit";
    return EXIT_SUCCESS;
}
