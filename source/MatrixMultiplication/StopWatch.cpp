#include "pch.h"
#include "StopWatch.h"

namespace MatrixMultiplication
{
	StopWatch::StopWatch() :
		mIsRunning(false), mElapsedTime(std::chrono::microseconds(0))
	{
	}
}