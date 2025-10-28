#!/usr/bin/env node

const { execSync, spawn } = require('child_process');
const path = require('path');
const fs = require('fs');
const os = require('os');

// Configuration
const nodeVersions = ['16.0.0', '18.0.0', '20.0.0', '22.0.0'];
const targetConfigs = [
    // Current platform builds
    { platform: 'darwin', arch: 'x64', nodeArch: 'x64' },
    { platform: 'darwin', arch: 'arm64', nodeArch: 'arm64' },
    { platform: 'linux', arch: 'x64', nodeArch: 'x64' },
    { platform: 'linux', arch: 'arm64', nodeArch: 'arm64' },
    { platform: 'win32', arch: 'x64', nodeArch: 'x64' },
    { platform: 'win32', arch: 'ia32', nodeArch: 'ia32' }
];

// Get current platform info
const currentPlatform = process.platform;
const currentArch = process.arch === 'x64' ? 'x64' : process.arch === 'arm64' ? 'arm64' : process.arch;

console.log(`Running prebuild script on ${currentPlatform}-${currentArch}`);

// Create prebuilds directory
const prebuildsDir = path.join(__dirname, '..', 'prebuilds');
if (!fs.existsSync(prebuildsDir)) {
    fs.mkdirSync(prebuildsDir, { recursive: true });
}

function canBuildForTarget(targetPlatform, targetArch) {
    // Check if we can build for the target platform/arch combination
    if (currentPlatform === targetPlatform) {
        // Same platform - check architecture compatibility
        if (targetPlatform === 'darwin') {
            // macOS supports cross-compilation between x64 and arm64
            return targetArch === 'x64' || targetArch === 'arm64';
        } else if (targetPlatform === 'linux') {
            // Linux can potentially cross-compile with proper toolchain
            return targetArch === currentArch; // For now, only same arch
        } else if (targetPlatform === 'win32') {
            // Windows cross-compilation from other platforms is complex
            return targetArch === currentArch;
        }
    }
    
    // Cross-platform compilation is generally not supported without containers/VMs
    return false;
}

function buildForTarget(targetPlatform, targetArch, nodeArch) {
    console.log(`\n=== Building for ${targetPlatform}-${targetArch} ===`);
    
    if (!canBuildForTarget(targetPlatform, targetArch)) {
        console.log(`⏭️  Skipping ${targetPlatform}-${targetArch} (cross-platform build not supported on current system)`);
        return false;
    }
    
    try {
        // Clean previous build
        console.log('Cleaning previous build...');
        try {
            execSync('cmake-js clean', { stdio: 'inherit' });
        } catch (cleanError) {
            console.warn('Clean failed, continuing...');
        }
        
        // Prepare build command with platform-specific options
        let buildCmd = `cmake-js build`;
        
        // Add architecture specification
        if (nodeArch) {
            buildCmd += ` --arch=${nodeArch}`;
        }
        
        // Add platform-specific CMake options
        if (targetPlatform === 'darwin' && targetArch !== currentArch) {
            // Cross-compilation on macOS
            const cmakeArch = targetArch === 'x64' ? 'x86_64' : 'arm64';
            buildCmd += ` --CDCMAKE_OSX_ARCHITECTURES=${cmakeArch}`;
        }
        
        console.log(`Running: ${buildCmd}`);
        execSync(buildCmd, { 
            stdio: 'inherit',
            env: {
                ...process.env,
                npm_config_target_platform: targetPlatform,
                npm_config_target_arch: nodeArch,
                npm_config_cache: path.join(os.homedir(), '.npm'),
                npm_config_build_from_source: 'true'
            }
        });
        
        // Find the built addon
        const buildDir = path.join(__dirname, '..', 'build');
        const releaseDir = path.join(buildDir, 'Release');
        
        // Look for addon file with different possible names
        const possibleAddonNames = [
            'fert-node.node',
            'fert_node.node',
            `fert-node.${targetPlatform === 'win32' ? 'dll' : 'node'}`
        ];
        
        let addonFile = null;
        for (const name of possibleAddonNames) {
            const candidate = path.join(releaseDir, name);
            if (fs.existsSync(candidate)) {
                addonFile = candidate;
                break;
            }
        }
        
        if (addonFile && fs.existsSync(addonFile)) {
            // Create prebuild directory structure compatible with prebuild-install
            const targetDir = path.join(prebuildsDir, `${targetPlatform}-${targetArch}`);
            
            if (!fs.existsSync(targetDir)) {
                fs.mkdirSync(targetDir, { recursive: true });
            }
            
            // Copy the addon to prebuilds with the expected name
            const targetFile = path.join(targetDir, 'node.napi.node');
            fs.copyFileSync(addonFile, targetFile);
            console.log(`✅ Prebuild created: ${targetFile}`);
            
            // Create a manifest file for this prebuild
            const manifest = {
                platform: targetPlatform,
                arch: targetArch,
                nodeArch: nodeArch,
                buildTime: new Date().toISOString(),
                nodeVersions: nodeVersions
            };
            
            fs.writeFileSync(
                path.join(targetDir, 'manifest.json'), 
                JSON.stringify(manifest, null, 2)
            );
            
            return true;
        } else {
            console.error(`❌ Addon file not found in: ${releaseDir}`);
            if (fs.existsSync(releaseDir)) {
                console.log('Available files:', fs.readdirSync(releaseDir));
            }
            return false;
        }
        
    } catch (error) {
        console.error(`❌ Failed to build for ${targetPlatform}-${targetArch}:`, error.message);
        return false;
    }
}

// Parse command line arguments
const args = process.argv.slice(2);
const buildAll = args.includes('--all');
const targetPlatformArg = args.find(arg => arg.startsWith('--platform='))?.split('=')[1];
const targetArchArg = args.find(arg => arg.startsWith('--arch='))?.split('=')[1];

let targetsToBuild = [];

if (buildAll) {
    // Build for all supported targets
    targetsToBuild = targetConfigs.filter(config => 
        canBuildForTarget(config.platform, config.arch)
    );
} else if (targetPlatformArg || targetArchArg) {
    // Build for specific platform/arch
    targetsToBuild = targetConfigs.filter(config => {
        const platformMatch = !targetPlatformArg || config.platform === targetPlatformArg;
        const archMatch = !targetArchArg || config.arch === targetArchArg;
        return platformMatch && archMatch && canBuildForTarget(config.platform, config.arch);
    });
} else {
    // Build for current platform only
    targetsToBuild = targetConfigs.filter(config => 
        config.platform === currentPlatform && canBuildForTarget(config.platform, config.arch)
    );
}

console.log('\n🚀 Starting prebuild process...');
console.log(`Will build for ${targetsToBuild.length} target(s):`);
targetsToBuild.forEach(config => {
    console.log(`  - ${config.platform}-${config.arch}`);
});

let successCount = 0;
let totalCount = targetsToBuild.length;

// Build for each target
for (const config of targetsToBuild) {
    const success = buildForTarget(config.platform, config.arch, config.nodeArch);
    if (success) successCount++;
}

console.log('\n📊 Prebuild Summary');
console.log('=' * 50);
console.log(`✅ Successful builds: ${successCount}/${totalCount}`);

if (fs.existsSync(prebuildsDir)) {
    const platforms = fs.readdirSync(prebuildsDir);
    platforms.forEach(platform => {
        const platformDir = path.join(prebuildsDir, platform);
        if (fs.statSync(platformDir).isDirectory()) {
            const files = fs.readdirSync(platformDir);
            const nodeFiles = files.filter(f => f.endsWith('.node'));
            console.log(`📦 ${platform}: ${nodeFiles.length} addon(s)`);
        }
    });
} else {
    console.log('❌ No prebuilds created');
}

console.log('\n💡 Usage examples:');
console.log('  npm run prebuild              # Build for current platform');
console.log('  npm run prebuild -- --all     # Build for all supported targets');
console.log('  npm run prebuild -- --platform=linux --arch=x64');
console.log('  npm run prebuild -- --arch=arm64');

if (successCount < totalCount) {
    console.log('\n⚠️  Some builds failed. This is normal if cross-compilation tools are not available.');
    console.log('Consider using CI/CD with multiple platform runners for complete coverage.');
}
