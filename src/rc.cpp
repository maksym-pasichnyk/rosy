#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

namespace {
	inline void replace(char *arg) {
		char *p = arg;
		while (*p) {
			if (!isalnum(*p)) {
				*p = '_';
			}
			p++;
		}
	}
}

auto parse_arguments(int argc, char** argv) -> std::vector<std::string> {
    std::vector<std::string> files{};
    files.reserve(argc - 2);
    for (int i = 3; i < argc; i++) {
        auto file(std::filesystem::relative(argv[i], argv[2]).string());

        replace(argv[i]);
        replace(file.data());

        files.emplace_back(std::move(file));
    }
    return std::move(files);
}

int main(int argc, char** argv) {
    std::filesystem::path output_directory{argv[1]};
    std::filesystem::path relative_directory{argv[2]};

    if (!std::filesystem::exists(output_directory)) {
        std::filesystem::create_directories(output_directory);
    }

    auto resources = parse_arguments(argc, argv);

    auto header = output_directory / "R.h";
    auto source = output_directory / "R.cpp";
    {
		std::ofstream file(header);
		file << R"(#pragma once
using Resource = struct { const char* _start, *_end; };
namespace R {
    enum class id {
)";
		for (auto const& resource : resources) {
            file << "		" << resource << ',' << std::endl;
		}
        file << "   };" << std::endl;
        file << "}" << std::endl;
        file.close();
	}

	{
		std::ofstream file(source);
        file << R"(
#include "R.h"
#include <string_view>
)" << std::endl;

        file << "namespace {" << std::endl;
        for (int i = 3; i < argc; i++) {
            file << "   extern \"C\" const char _binary_" << argv[i] << "_start;" << std::endl;
            file << "   extern \"C\" const char _binary_" << argv[i] << "_end;" << std::endl;
        }

        file << "   Resource _binary_resources[] = {" << std::endl;
        for (int i = 3; i < argc; i++) {
            file << "       { &_binary_" << argv[i] << "_start, &_binary_" << argv[i] << "_end }," << std::endl;
        }
        file << "   };" << std::endl;
        file << "}" << std::endl;
        file << R"(
namespace rosy::resources {
    auto get_string(R::id id) -> std::string_view {
        const auto src = _binary_resources[static_cast<size_t>(id)];
        return std::string_view(src._start, src._end - src._start);
    }
}
        )";
        file.close();
	}

	return 0;
}