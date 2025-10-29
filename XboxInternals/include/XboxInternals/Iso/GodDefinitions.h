#pragma once
#include <cstdint>

namespace XboxInternals::Iso {

// Games on Demand (GOD) format constants (based on XGDTool)
namespace God {
    constexpr uint64_t BLOCK_SIZE = 0x1000;  // 4096 bytes per block
    constexpr uint32_t DATA_BLOCKS_PER_PART = 0xA290;  // Data blocks per part file
    constexpr uint32_t DATA_BLOCKS_PER_SHT = 0xCB;     // Data blocks per sub hash table
    constexpr uint32_t HASH_ENTRY_SIZE = 0x18;         // 24 bytes per hash entry
    constexpr uint32_t MAX_PART_SIZE = 0xB0000000;     // ~2.75 GB max per part

    // Hash table sizes
    constexpr uint32_t MASTER_HASH_SIZE = BLOCK_SIZE;
    constexpr uint32_t SUB_HASH_TABLE_SIZE = BLOCK_SIZE;
}

// GOD file structure:
// Part 0:
//   - Master hash table (1 block)
//   - Sub hash table 0 (1 block)
//   - Data blocks (0xCB blocks)
//   - Sub hash table 1 (1 block)
//   - Data blocks (0xCB blocks)
//   - ... continues until 0xA290 data blocks
// Part 1, 2, etc follow same pattern

struct GodRemap {
    uint64_t offset;      // Byte offset in GOD part file
    uint32_t fileIndex;   // Which part file (0, 1, 2, etc)
};

// Remap XISO sector to GOD offset
inline GodRemap remapSectorToGod(uint64_t xisoSector) {
    uint64_t blockNum = (xisoSector * 0x800) / God::BLOCK_SIZE;
    uint32_t fileIndex = static_cast<uint32_t>(blockNum / God::DATA_BLOCKS_PER_PART);
    uint64_t dataBlockWithinFile = blockNum % God::DATA_BLOCKS_PER_PART;
    uint32_t hashIndex = static_cast<uint32_t>(dataBlockWithinFile / God::DATA_BLOCKS_PER_SHT);

    uint64_t remappedOffset = God::BLOCK_SIZE; // master hash table
    remappedOffset += ((hashIndex + 1) * God::BLOCK_SIZE); // sub hash tables
    remappedOffset += (dataBlockWithinFile * God::BLOCK_SIZE); // data blocks
    remappedOffset += (xisoSector * 0x800) % God::BLOCK_SIZE; // offset within block

    return { remappedOffset, fileIndex };
}

// Remap GOD offset back to XISO sector
inline uint64_t remapGodToSector(const GodRemap& remap) {
    uint64_t dataBlocksBeforePart = remap.fileIndex * God::DATA_BLOCKS_PER_PART;
    
    // Calculate which sub hash table we're in
    uint64_t offsetInFile = remap.offset - God::BLOCK_SIZE; // Skip master hash
    uint32_t subHashIndex = 0;
    
    while (offsetInFile >= God::SUB_HASH_TABLE_SIZE) {
        offsetInFile -= God::SUB_HASH_TABLE_SIZE;
        if (offsetInFile >= God::DATA_BLOCKS_PER_SHT * God::BLOCK_SIZE) {
            offsetInFile -= God::DATA_BLOCKS_PER_SHT * God::BLOCK_SIZE;
            subHashIndex++;
        } else {
            break;
        }
    }
    
    uint64_t dataBlockInFile = (subHashIndex * God::DATA_BLOCKS_PER_SHT) + (offsetInFile / God::BLOCK_SIZE);
    uint64_t totalBlock = dataBlocksBeforePart + dataBlockInFile;
    uint64_t byteOffset = (totalBlock * God::BLOCK_SIZE) + (offsetInFile % God::BLOCK_SIZE);
    
    return byteOffset / 0x800;
}

} // namespace XboxInternals::Iso
