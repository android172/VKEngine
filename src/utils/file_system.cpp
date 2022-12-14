#include "file_system.hpp"

FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}

Result<Vector<byte>, RuntimeError> FileSystem::read_file_bytes(
    const String& file_path
) {
    std::ifstream file { file_path,
                         std::ios::ate | std::ios::binary | std::ios::in };

    if (!file.is_open()) return Failure("Failed to open file: " + file_path);

    size_t       file_size = static_cast<size_t>(file.tellg());
    Vector<byte> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();

    return buffer;
}

Result<Vector<String>, RuntimeError> FileSystem::read_file_lines(
    const String& file_path
) {
    std::ifstream file(file_path, std::ios::in);

    if (!file.is_open()) return Failure("Failed to open file: " + file_path);

    Vector<String> lines;
    String         line;
    while (std::getline(file, line))
        lines.push_back(line);

    file.close();

    return lines;
}