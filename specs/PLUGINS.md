# Velocity-Next Plugin Architecture

**Status**: Concept  
**Target Version**: TBD (0.5.0.0+)

## Overview

Plugin system to extend Velocity-Next functionality without modifying core code. Enables community contributions, specialized tools, and optional features that don't belong in the main application.

## Motivation

**Benefits:**
- Community can create specialized tools
- Keep core application lean
- Optional features don't bloat main binary
- Easy to distribute/share tools
- Experimental features can be tested independently

## Plugin Candidates

### 1. ISO Patcher Plugin
**Based on:** https://github.com/Qubits01/patch_xiso (MIT)

**Features:**
- Patch ISOs to match Redump.org standards
- Fix overdump/underdump issues
- Zero-fill L0 Video padding
- Overwrite stealth sectors with zeros
- Archive-quality ISO preparation
- Redump.org database integration

**Use Cases:**
- Preparing ISOs for archival
- Fixing bad dumps
- Verifying ISO integrity
- Matching preservation standards

**Integration:**
- Menu: Tools → Plugins → ISO Patcher
- Right-click ISO → Patch to Redump Standard
- Batch patching support
- Before/after comparison

### 2. Future Plugin Ideas

**Game Database Plugin:**
- XboxUnity.net integration
- Automatic metadata lookup
- Cover art downloading
- Achievement guides

**FTP Transfer Plugin:**
- Connect to Xbox 360 via FTP
- Browse console HDD
- Transfer GOD packages
- Install content remotely

**Disc Burning Plugin:**
- Burn ISOs to DVD±R DL
- Verify burned discs
- Optimal write speed detection
- Layer break positioning

**Region Conversion Plugin:**
- Convert game regions
- Patch region checks
- Multi-region enabler

**Achievement Editor Plugin:**
- Create custom achievements
- Edit achievement icons
- Import/export achievement sets
- Test achievement unlocks

**Save Game Editor Plugin:**
- Decrypt/encrypt save files
- Edit game progress
- Unlock items/levels
- Backup/restore saves

## Plugin Architecture Design

### Plugin Interface (Future Design)

```cpp
// VelocityPlugin.h
class VelocityPlugin {
public:
    virtual ~VelocityPlugin() = default;
    
    // Plugin metadata
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString author() const = 0;
    virtual QString description() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Integration points
    virtual QList<QAction*> menuActions() = 0;
    virtual QList<QAction*> contextActions(const QString& fileType) = 0;
    
    // Processing
    virtual bool canHandle(const QString& filePath) = 0;
    virtual void process(const QString& filePath) = 0;
};
```

### Plugin Manifest (JSON)

```json
{
  "name": "ISO Patcher",
  "id": "com.velocitynext.plugin.isopatcher",
  "version": "1.0.0",
  "author": "Community",
  "description": "Patch Xbox 360 ISOs to Redump standards",
  "license": "MIT",
  "requires": {
    "velocity": ">=0.3.0",
    "features": ["iso-support"]
  },
  "entry": "isopatcher.dll",
  "config": "config.json"
}
```

### Plugin Directory Structure

```
Velocity-Next/
├── plugins/
│   ├── iso-patcher/
│   │   ├── manifest.json
│   │   ├── isopatcher.dll (or .so/.dylib)
│   │   ├── config.json
│   │   ├── LICENSE
│   │   └── README.md
│   ├── game-database/
│   └── ftp-client/
```

## Implementation Considerations

### Security
- Plugin sandboxing
- Code signing for trusted plugins
- Permissions system (file access, network, etc.)
- User confirmation for untrusted plugins

### Distribution
- Official plugin repository
- GitHub releases
- In-app plugin browser/installer
- Automatic updates

### API Stability
- Versioned plugin API
- Backward compatibility guarantees
- Deprecation warnings
- Migration guides

### Performance
- Lazy loading (load on demand)
- Unload unused plugins
- Memory isolation
- No impact on startup time

## Technical Approach

### Option 1: Qt Plugin System
- Use Qt's built-in plugin framework
- Q_PLUGIN_METADATA for discovery
- QPluginLoader for dynamic loading
- Type-safe interfaces

### Option 2: Scripting (Python/Lua)
- Embed Python/Lua interpreter
- Script-based plugins
- Easier for community development
- Slower than native plugins

### Option 3: WebAssembly
- Sandboxed execution
- Cross-platform binary distribution
- Performance close to native
- Limited system access (good for security)

## References

**Existing Plugin Systems:**
- Qt Plugin Framework: https://doc.qt.io/qt-6/plugins-howto.html
- OBS Studio plugins: https://obsproject.com/
- VS Code extensions: https://code.visualstudio.com/api
- Audacity plugins: https://www.audacityteam.org/

**Plugin Candidates:**
- patch_xiso: https://github.com/Qubits01/patch_xiso
- Redump.org: http://redump.org

## Development Roadmap

1. **Phase 1: Core API** (v0.5.0)
   - Define plugin interface
   - Implement loader/discovery
   - Basic lifecycle management
   
2. **Phase 2: Integration** (v0.6.0)
   - Menu integration
   - Context menu support
   - Settings/preferences
   
3. **Phase 3: First Plugin** (v0.7.0)
   - ISO Patcher as reference implementation
   - Documentation for plugin developers
   - Plugin SDK/template
   
4. **Phase 4: Ecosystem** (v0.8.0)
   - Plugin repository
   - In-app browser/installer
   - Community contributions

## Related Features

- 003-iso-xex-support: Foundation for ISO plugins
- 004-iso-god-conversion: Conversion plugins
- Future: Scripting API for automation

## Notes

- Keep core application stable and focused
- Plugins handle specialized/experimental features
- Community-driven development model
- MIT-licensed reference plugins encourage contributions
- Plugin API must be stable and well-documented
- Consider security implications carefully
