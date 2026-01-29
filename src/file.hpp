#pragma once
#include <filesystem>

std::filesystem::path get_executable_dir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#else
    return std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
}

std::optional<std::string> read_file(const std::string& filename) {
	
	std::filesystem::path exe_dir = get_executable_dir();
	std::filesystem::path full_path = exe_dir / filename;
	
	std::ifstream fileStream(full_path, std::ios::binary);

	if (!fileStream.is_open()) {
		std::cerr << "ERROR: Failed to open file: " << filename << "\n";
		std::cerr << "Current working directory: " 
		          << std::filesystem::current_path() << "\n";
		
		// Try to show what files exist in common locations
		std::cerr << "Checking for files:\n";
		
		if (std::filesystem::exists(filename)) {
			std::cerr << "  [EXISTS] " << filename << "\n";
		} else {
			std::cerr << "  [NOT FOUND] " << filename << "\n";
		}
		
		// Check if shaders directory exists
		if (std::filesystem::exists("shaders")) {
			std::cerr << "  Found 'shaders/' directory, contents:\n";
			for (const auto& entry : std::filesystem::directory_iterator("shaders")) {
				std::cerr << "    - " << entry.path().filename().string() << "\n";
			}
		} else {
			std::cerr << "  'shaders/' directory does not exist\n";
		}
		
		// Check parent directories
		std::filesystem::path parent_shaders = "../shaders";
		if (std::filesystem::exists(parent_shaders)) {
			std::cerr << "  Found '../shaders/' directory\n";
		}
		
		return std::nullopt;
	}

	try {
		std::string content(
			(std::istreambuf_iterator<char>(fileStream)),
			std::istreambuf_iterator<char>()
		);

		if (fileStream.bad()) {
			std::cerr << "ERROR: Failed to read file: " << filename << "\n";
			return std::nullopt;
		}
		return content;
	} catch (const std::exception& e) {
		std::cerr << "ERROR: Exception reading file " << filename 
		          << ": " << e.what() << "\n";
		return std::nullopt;
	} catch (...) {
		std::cerr << "ERROR: Unknown exception reading file: " 
		          << filename << "\n";
		return std::nullopt;
	}
}