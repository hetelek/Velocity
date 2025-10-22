#include <iostream>
#include "XboxInternals/Gpd/GameGpd.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: test_gpd_thumbnail <gpd_file_path>" << std::endl;
        return 1;
    }

    try {
        GameGpd gpd(argv[1]);
        
        std::cout << "GPD loaded successfully" << std::endl;
        std::cout << "Game name: " << std::string(gpd.gameName.ws.begin(), gpd.gameName.ws.end()) << std::endl;
        std::cout << "Number of images: " << gpd.images.size() << std::endl;
        
        // Check for thumbnail (ID 0x8000)
        bool hasThumbnail = false;
        for (size_t i = 0; i < gpd.images.size(); i++) {
            std::cout << "  Image " << i << " - ID: 0x" << std::hex << gpd.images[i].entry.id 
                      << std::dec << ", Length: " << gpd.images[i].length << " bytes" << std::endl;
            if (gpd.images[i].entry.id == 0x8000) {
                hasThumbnail = true;
                std::cout << "  ^^^ This is the game thumbnail!" << std::endl;
            }
        }
        
        if (gpd.thumbnail.image != nullptr) {
            std::cout << "\nGPD thumbnail field is set!" << std::endl;
            std::cout << "Thumbnail length: " << gpd.thumbnail.length << " bytes" << std::endl;
        } else {
            std::cout << "\nGPD thumbnail field is NULL" << std::endl;
        }
        
        std::cout << "\nSummary: Thumbnail " << (hasThumbnail ? "EXISTS" : "MISSING") << " in this GPD file" << std::endl;
        
    } catch (const std::string& error) {
        std::cerr << "Error: " << error << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
