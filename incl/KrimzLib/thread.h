#pragma once


namespace kl {
	class thread {
	public:
		// Multithreaded for loop
		static void ParallelFor(int startInclusive, int endExclusive, int threadCount, std::function<void(int i)> loopBody) {
			// Thread storage
			std::vector<std::thread> cpuThreads(threadCount);
			int runsPerThread = (endExclusive - startInclusive) / threadCount;
			
			// Thread creation
			for (int i = 0; i < threadCount; i++) {
				int loopStart = runsPerThread * i + startInclusive;
				int loopEnd = (i == threadCount - 1) ? endExclusive : (loopStart + runsPerThread);
				cpuThreads[i] = std::thread([&](int start, int end) { for (int i = start; i < end; i++) { loopBody(i); } }, loopStart, loopEnd);
			}

			// Waiting for the threads to finish
			for (int i = 0; i < threadCount; i++) {
				cpuThreads[i].join();
			}
		}
	};
}