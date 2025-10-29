#include "XboxInternals/Plugins/IsoExporter.h"
#include "XboxInternals/Iso/IsoImage.h"
#include "XboxInternals/Iso/GodImage.h"
#include "XboxInternals/Iso/GodWriter.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace XboxInternals::Plugins {

// Batch creation plugin for ISO and GOD formats
class BatchCreator : public IsoExporter {
public:
    enum class OutputFormat {
        ISO,
        GOD
    };

    BatchCreator(OutputFormat format = OutputFormat::ISO) : outputFormat_(format) {}

    bool exportIso(const std::string& inputPath, const std::string& outDir) override {
        fs::path p(inputPath);
        
        if (!fs::exists(p)) return false;

        if (outputFormat_ == OutputFormat::ISO) {
            return createIso(inputPath, outDir);
        } else {
            return createGod(inputPath, outDir);
        }
    }

    std::string name() const override { 
        return outputFormat_ == OutputFormat::ISO ? 
            "Batch Creator (ISO)" : "Batch Creator (GOD)"; 
    }

    void setOutputFormat(OutputFormat format) { outputFormat_ = format; }

private:
    OutputFormat outputFormat_;

    bool createIso(const std::string& inputPath, const std::string& outDir) {
        fs::path input(inputPath);
        fs::path output(outDir);

        // Determine output filename
        std::string outputName = input.stem().string() + ".iso";
        fs::path outputIso = output / outputName;

        // Create ISO from directory or convert from existing ISO
        if (fs::is_directory(input)) {
            return XboxInternals::Iso::IsoImage::createFromDirectory(inputPath, outputIso.string());
        } else {
            // Could be GOD->ISO conversion or ISO->ISO copy
            std::error_code ec;
            fs::create_directories(output, ec);
            if (ec) return false;
            
            return XboxInternals::Iso::IsoImage::createFromIso(inputPath, outputIso.string());
        }
    }

    bool createGod(const std::string& inputPath, const std::string& outDir) {
        fs::path input(inputPath);
        fs::path output(outDir);

        XboxInternals::Iso::GodWriter writer;

        // Create GOD from directory or ISO
        if (fs::is_directory(input)) {
            return writer.createFromDirectory(inputPath, outDir);
        } else {
            std::string ext = input.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".iso") {
                return writer.createFromIso(inputPath, outDir);
            }
        }

        return false;
    }
};

// Factory functions
IsoExporter* createBatchIsoCreator() { return new BatchCreator(BatchCreator::OutputFormat::ISO); }
IsoExporter* createBatchGodCreator() { return new BatchCreator(BatchCreator::OutputFormat::GOD); }

} // namespace XboxInternals::Plugins
