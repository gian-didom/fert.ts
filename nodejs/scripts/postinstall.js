#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const os = require('os');

const platform = os.platform();
const arch = os.arch();

// Map Node.js arch to prebuild arch
const archMap = {
  'x64': 'x64',
  'arm64': 'arm64',
  'ia32': 'ia32'
};

const prebuildArch = archMap[arch] || arch;
const prebuildDir = path.join(__dirname, '..', 'prebuilds', `${platform}-${prebuildArch}`);
const targetDir = path.join(__dirname, '..', 'build', 'Release');

console.log(`Looking for prebuilt binary in: ${prebuildDir}`);

if (fs.existsSync(prebuildDir)) {
  // Find the .node file
  const files = fs.readdirSync(prebuildDir);
  const nodeFile = files.find(f => f.endsWith('.node'));
  
  if (nodeFile) {
    const source = path.join(prebuildDir, nodeFile);
    
    // Create target directory if it doesn't exist
    if (!fs.existsSync(targetDir)) {
      fs.mkdirSync(targetDir, { recursive: true });
    }
    
    const target = path.join(targetDir, 'fert-node.node');
    
    console.log(`Copying prebuilt binary: ${source} -> ${target}`);
    fs.copyFileSync(source, target);
    console.log('✓ Prebuilt binary installed successfully');
    process.exit(0);
  }
}

console.error('\n❌ No prebuilt binary found for your platform!');
console.error(`   Platform: ${platform}, Architecture: ${arch}`);
console.error('\nThis package requires a prebuilt binary for your platform.');
console.error('Please contact the package maintainer or build from source.\n');
process.exit(1);
