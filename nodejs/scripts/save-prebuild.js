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
const sourceFile = path.join(__dirname, '..', 'build', 'Release', 'fert-node.node');

if (!fs.existsSync(sourceFile)) {
  console.error('Error: Built binary not found at:', sourceFile);
  console.error('Please run `npm run build` first');
  process.exit(1);
}

// Create prebuild directory
if (!fs.existsSync(prebuildDir)) {
  fs.mkdirSync(prebuildDir, { recursive: true });
}

const targetFile = path.join(prebuildDir, 'node.napi.node');

console.log(`Saving prebuilt binary: ${sourceFile} -> ${targetFile}`);
fs.copyFileSync(sourceFile, targetFile);

console.log('✓ Prebuilt binary saved successfully');
console.log(`Location: ${prebuildDir}`);
