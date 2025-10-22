// Test harness for >4GB file support with modern C++20 std::fstream
// Compile: g++ -std=c++20 test_large_file.cpp -o test_large_file
// Run: ./test_large_file

#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <chrono>

int main() {
    using namespace std::chrono;
    
    const std::string testFile = "test_5gb.bin";
    const size_t chunkSize = 1024 * 1024;  // 1 MB
    const int numChunks = 5000;  // 5000 MB = ~5 GB
    
    std::cout << "=== Modern std::fstream >4GB Support Test ===\n";
    std::cout << "Compiler: " << __VERSION__ << "\n";
    std::cout << "Test file: " << testFile << "\n";
    std::cout << "Target size: " << numChunks << " MB (~5 GB)\n\n";
    
    // Clean up old test file
    if (std::filesystem::exists(testFile)) {
        std::filesystem::remove(testFile);
    }
    
    // === WRITE TEST ===
    std::cout << "Starting WRITE test...\n";
    auto startWrite = high_resolution_clock::now();
    
    {
        std::ofstream out(testFile, std::ios::binary);
        if (!out) {
            std::cerr << "FAIL: Could not open file for writing\n";
            return 1;
        }
        
        std::vector<char> buffer(chunkSize, 'X');
        
        for (int i = 0; i < numChunks; i++) {
            out.write(buffer.data(), buffer.size());
            
            if (out.fail()) {
                std::cerr << "FAIL: Write failed at " << (i + 1) << " MB\n";
                std::cerr << "      This confirms FatxDrive.cpp:576 comment about fstream limits\n";
                out.close();
                std::filesystem::remove(testFile);
                return 1;
            }
            
            // Progress every 500 MB
            if ((i + 1) % 500 == 0) {
                std::cout << "  Written: " << (i + 1) << " MB\n";
            }
        }
        
        out.close();
        if (out.fail()) {
            std::cerr << "FAIL: Close failed after writing\n";
            std::filesystem::remove(testFile);
            return 1;
        }
    }
    
    auto endWrite = high_resolution_clock::now();
    auto writeMs = duration_cast<milliseconds>(endWrite - startWrite).count();
    std::cout << "✓ WRITE test PASSED (" << writeMs << " ms)\n\n";
    
    // Verify file size
    auto actualSize = std::filesystem::file_size(testFile);
    auto expectedSize = static_cast<uintmax_t>(chunkSize) * numChunks;
    
    std::cout << "File size check:\n";
    std::cout << "  Expected: " << expectedSize << " bytes (" << (expectedSize / 1024 / 1024) << " MB)\n";
    std::cout << "  Actual:   " << actualSize << " bytes (" << (actualSize / 1024 / 1024) << " MB)\n";
    
    if (actualSize != expectedSize) {
        std::cerr << "FAIL: File size mismatch!\n";
        std::filesystem::remove(testFile);
        return 1;
    }
    std::cout << "✓ Size verification PASSED\n\n";
    
    // === READ TEST ===
    std::cout << "Starting READ test...\n";
    auto startRead = high_resolution_clock::now();
    
    {
        std::ifstream in(testFile, std::ios::binary);
        if (!in) {
            std::cerr << "FAIL: Could not open file for reading\n";
            std::filesystem::remove(testFile);
            return 1;
        }
        
        std::vector<char> buffer(chunkSize);
        size_t totalRead = 0;
        
        for (int i = 0; i < numChunks; i++) {
            in.read(buffer.data(), buffer.size());
            
            if (in.fail() && !in.eof()) {
                std::cerr << "FAIL: Read failed at " << (i + 1) << " MB\n";
                in.close();
                std::filesystem::remove(testFile);
                return 1;
            }
            
            totalRead += in.gcount();
            
            // Progress every 500 MB
            if ((i + 1) % 500 == 0) {
                std::cout << "  Read: " << (i + 1) << " MB\n";
            }
        }
        
        in.close();
        
        std::cout << "  Total read: " << totalRead << " bytes (" << (totalRead / 1024 / 1024) << " MB)\n";
        
        if (totalRead != expectedSize) {
            std::cerr << "FAIL: Did not read full file!\n";
            std::filesystem::remove(testFile);
            return 1;
        }
    }
    
    auto endRead = high_resolution_clock::now();
    auto readMs = duration_cast<milliseconds>(endRead - startRead).count();
    std::cout << "✓ READ test PASSED (" << readMs << " ms)\n\n";
    
    // === SEEK TEST (CRITICAL for FatxDrive) ===
    std::cout << "Starting SEEK test (critical for FatxDrive backup/restore)...\n";
    
    {
        std::fstream file(testFile, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            std::cerr << "FAIL: Could not open file for seek testing\n";
            std::filesystem::remove(testFile);
            return 1;
        }
        
        // Test seeking beyond 4GB boundary
        const std::streampos pos4GB = static_cast<std::streampos>(4ULL * 1024 * 1024 * 1024);  // Exactly 4 GB
        const std::streampos pos4GBplus = pos4GB + 100;  // 4GB + 100 bytes
        
        std::cout << "  Seeking to 4GB + 100 bytes...\n";
        file.seekg(pos4GBplus);
        
        if (file.fail()) {
            std::cerr << "FAIL: Seek beyond 4GB failed!\n";
            std::cerr << "      This is likely the issue mentioned in FatxDrive.cpp:576\n";
            file.close();
            std::filesystem::remove(testFile);
            return 1;
        }
        
        auto currentPos = file.tellg();
        std::cout << "  Current position: " << currentPos << " (expected: " << pos4GBplus << ")\n";
        
        if (currentPos != pos4GBplus) {
            std::cerr << "FAIL: Position mismatch after seek!\n";
            file.close();
            std::filesystem::remove(testFile);
            return 1;
        }
        
        file.close();
    }
    
    std::cout << "✓ SEEK test PASSED\n\n";
    
    // Cleanup
    std::filesystem::remove(testFile);
    
    std::cout << "=== ALL TESTS PASSED ===\n";
    std::cout << "✓ Modern std::fstream CAN handle >4GB files on this system\n";
    std::cout << "✓ Recommendation: Replace platform-specific file I/O with std::fstream\n";
    std::cout << "✓ FatxDrive.cpp comment at line 576 may be outdated for modern C++20\n";
    
    return 0;
}


