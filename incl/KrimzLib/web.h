#pragma once

#include <string>

#include "KrimzLib/file.h"


namespace kl {
	namespace web {
		// Downloads website data(bytes) from the given url
		std::string getWebsiteData(const std::string& url, int bufferSize = 65536);

		// Downloads data from the website and saves it in a file
		void downloadToFile(const std::string& url, const std::string& fileName);
	};
}
