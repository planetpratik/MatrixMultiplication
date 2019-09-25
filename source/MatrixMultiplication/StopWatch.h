#pragma once

#include <chrono>

namespace MatrixMultiplication
{
	class StopWatch final
	{
	public:
		StopWatch();
		~StopWatch() = default;
		StopWatch(const StopWatch& rhs) = delete;
		StopWatch& operator=(const StopWatch& rhs) = delete;
		StopWatch(StopWatch&& rhs) = default;
		StopWatch& operator=(StopWatch&& rhs) = default;

		const std::chrono::microseconds& Elapsed() const;
		const std::chrono::milliseconds ElapsedMilliseconds() const;
		const std::chrono::seconds ElapsedSeconds() const;
		bool IsRunning() const;

		void Reset();
		void Restart();
		void Start();
		void Stop();

	private:
		std::chrono::high_resolution_clock::time_point mStartTime;
		std::chrono::microseconds mElapsedTime;
		bool mIsRunning;
	};
}

#include "StopWatch.inl"
