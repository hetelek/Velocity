#pragma once
#include <string>

namespace XboxInternals::Plugins {

class IsoExporter {
public:
    virtual ~IsoExporter() = default;

    // Export an ISO to the output directory. Return true on success.
    virtual bool exportIso(const std::string& isoPath, const std::string& outDir) = 0;

    // Human-readable exporter name
    virtual std::string name() const = 0;
};

} // namespace XboxInternals::Plugins
