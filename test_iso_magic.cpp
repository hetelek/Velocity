#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <cstdint>
#include <algorithm>

int main() {
    const char* path = "I:\\CONSOLE\\xbox360\\Lips [RF].iso";
    const char* magic = "MICROSOFT*XBOX*MEDIA";
    const size_t magic_len = 20;
    
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return 1;
    }
    
    auto size = file.tellg();
    std::cout << "File size: " << size << " bytes (" << (size / 1024 / 1024) << " MB)" << std::endl;
    
    file.seekg(0, std::ios::beg);
    
    // Check offsets
    const uint64_t offsets[] = {
        0x10000,        // Standard GDF
        0x02080000,     // XGD3
        0x18300000      // XGD1
    };
    
    for (auto offset : offsets) {
        std::cout << "\n=== Checking offset 0x" << std::hex << offset << std::dec << " (" << offset << ") ===" << std::endl;
        
        if (offset + magic_len > (uint64_t)size) {
            std::cout << "  Offset beyond file size, skipping" << std::endl;
            continue;
        }
        
        file.seekg(offset, std::ios::beg);
        if (!file.good()) {
            std::cout << "  Seek failed" << std::endl;
            continue;
        }
        
        char buffer[256] = {0};
        file.read(buffer, 128);
        auto bytes_read = file.gcount();
        
        std::cout << "  Read " << bytes_read << " bytes" << std::endl;
        std::cout << "  First 64 bytes as hex:" << std::endl << "  ";
        for (int i = 0; i < 64 && i < bytes_read; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)(unsigned char)buffer[i] << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl << "  ";
        }
        std::cout << std::dec << std::endl;
        
        std::cout << "  First " << magic_len << " bytes as string: ";
        for (int i = 0; i < magic_len && i < bytes_read; i++) {
            if (buffer[i] >= 32 && buffer[i] < 127) {
                std::cout << buffer[i];
            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
        
        if (std::memcmp(buffer, magic, magic_len) == 0) {
            std::cout << "  ✓ MAGIC STRING FOUND!" << std::endl;
            
            // Read root directory info
            uint32_t rootSector = 0, rootSize = 0;
            file.read((char*)&rootSector, 4);
            file.read((char*)&rootSize, 4);
            std::cout << "  Root sector: 0x" << std::hex << rootSector << std::dec << " (" << rootSector << ")" << std::endl;
            std::cout << "  Root size: " << rootSize << " bytes" << std::endl;
        } else {
            std::cout << "  ✗ Magic string does not match" << std::endl;
            std::cout << "  Expected: " << magic << std::endl;
        }
    }
    
    // Scan for magic string anywhere in first 100MB
    std::cout << "\n=== Scanning entire file for magic string ===" << std::endl;
    file.seekg(0, std::ios::beg);
    
    const size_t scan_size = (uint64_t)size;
    const size_t buffer_size = 1024 * 1024; // 1MB buffer
    char* scan_buffer = new char[buffer_size + magic_len];
    
    int found_count = 0;
    for (size_t pos = 0; pos < scan_size && found_count < 5; pos += buffer_size) {
        file.seekg(pos, std::ios::beg);
        size_t to_read = std::min(buffer_size + magic_len, scan_size - pos);
        file.read(scan_buffer, to_read);
        auto read = file.gcount();
        
        for (size_t i = 0; i < read - magic_len; i++) {
            if (std::memcmp(scan_buffer + i, magic, magic_len) == 0) {
                std::cout << "  Found at offset: 0x" << std::hex << (pos + i) << std::dec 
                         << " (" << (pos + i) << ")" << std::endl;
                found_count++;
                if (found_count >= 5) break;
            }
        }
        
        // Progress indicator
        if ((pos / buffer_size) % 100 == 0) {
            std::cout << "  Scanned " << (pos / 1024 / 1024) << " MB..." << std::endl;
        }
    }
    
    if (found_count == 0) {
        std::cout << "  No magic string found in entire file!" << std::endl;
        std::cout << "\n  This might be:" << std::endl;
        std::cout << "  - A redump/scrubbed ISO (padding removed)" << std::endl;
        std::cout << "  - An extracted/rebuilt ISO" << std::endl;
        std::cout << "  - A different Xbox format (XSF, CCI, etc.)" << std::endl;
        
        // Check what's at offset 0
        file.seekg(0, std::ios::beg);
        char header[64];
        file.read(header, 64);
        std::cout << "\n  First 64 bytes of file:" << std::endl << "  ";
        for (int i = 0; i < 64; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)(unsigned char)header[i] << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl << "  ";
        }
        std::cout << std::dec << std::endl;
    }
    
    delete[] scan_buffer;
    
    std::cout << "\nDone." << std::endl;
    return 0;
}
