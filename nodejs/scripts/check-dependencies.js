#!/usr/bin/env node

const { execSync } = require('child_process');
const os = require('os');

function checkYamlCpp() {
  const platform = os.platform();
  
  console.log('Checking for yaml-cpp library...');
  
  try {
    if (platform === 'linux') {
      // Check using ldconfig
      execSync('ldconfig -p | grep libyaml-cpp', { stdio: 'pipe' });
      console.log('✓ yaml-cpp found');
      return true;
    } else if (platform === 'darwin') {
      // Check using brew or system libraries
      try {
        execSync('brew list yaml-cpp', { stdio: 'pipe' });
        console.log('✓ yaml-cpp found (via Homebrew)');
        return true;
      } catch (e) {
        execSync('ls /usr/local/lib/libyaml-cpp* || ls /opt/homebrew/lib/libyaml-cpp*', { stdio: 'pipe' });
        console.log('✓ yaml-cpp found');
        return true;
      }
    }
  } catch (error) {
    console.error('\n❌ yaml-cpp library not found!\n');
    console.error('Please install yaml-cpp before using this package:\n');
    
    if (platform === 'linux') {
      console.error('Ubuntu/Debian:');
      console.error('  sudo apt-get install libyaml-cpp-dev\n');
      console.error('Fedora/RHEL:');
      console.error('  sudo dnf install yaml-cpp-devel\n');
    } else if (platform === 'darwin') {
      console.error('macOS (Homebrew):');
      console.error('  brew install yaml-cpp\n');
    }
    
    process.exit(1);
  }
}

checkYamlCpp();
