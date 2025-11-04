# FERT Node.js Wrapper

Fast Ephemeris Retrieval Tool (FERT) Node.js bindings for high-performance orbital mechanics calculations.

## Installation

### From GitHub Packages

#### System Prerequisites

FERT Node.js requires yaml-cpp library to be installed on your system:

**Ubuntu/Debian:**
```bash
sudo apt-get install libyaml-cpp-dev
```

**macOS (Homebrew):**
```bash
brew install yaml-cpp
```

**Fedora/RHEL:**
```bash
sudo dnf install yaml-cpp-devel
```

#### Install Package

First, configure npm to use GitHub Packages for `@dartlab` scope:

```bash
# Create or edit ~/.npmrc
echo "@dartlab:registry=https://npm.pkg.github.com" >> ~/.npmrc
```

Then authenticate with GitHub (you need a Personal Access Token with `read:packages` scope):

```bash
npm login --scope=@dartlab --registry=https://npm.pkg.github.com
# Username: your-github-username
# Password: your-github-personal-access-token
# Email: your-email@example.com
```

Install the package:

```bash
npm install @dartlab/fert-node
```

### Build from Source

#### Prerequisites

- Node.js 14+ 
- CMake 3.15+
- C++17 compatible compiler
- FERT C++ library dependencies

### Build

```bash
cd nodejs
npm install
npm run build
```

### Configuration

The build uses the following CMake configuration as specified:

```bash
cmake -D CMAKE_OSX_ARCHITECTURES=x86_64 \
      -D CMAKE_PREFIX_PATH="/usr/local" \
      -D CMAKE_CXX_FLAGS="-march=x86-64-v3"
```

## Usage

```javascript
const { Fert, SPICE_IDS } = require('fert-node');

// Initialize with metakernel
const fert = new Fert('/path/to/metakernel.yaml');

// Get Earth position relative to Solar System Barycenter at J2000
const position = fert.getState(0, SPICE_IDS.EARTH, SPICE_IDS.SOLAR_SYSTEM_BARYCENTER, SPICE_IDS.J2000);
console.log('Earth position:', position); // [x, y, z] in km

// Get state with velocity
const state = fert.getStateWithVelocity(0, SPICE_IDS.EARTH, SPICE_IDS.SOLAR_SYSTEM_BARYCENTER, SPICE_IDS.J2000);
console.log('Position:', state.position); // [x, y, z] in km  
console.log('Velocity:', state.velocity); // [vx, vy, vz] in km/s

// Get light-time corrected state
const correctedState = fert.getStateLightTime(0, SPICE_IDS.MOON, SPICE_IDS.EARTH, SPICE_IDS.J2000);
console.log('Moon position (LT corrected):', correctedState.position);
console.log('Light time:', correctedState.lightTime); // seconds
```

## API Reference

### Class: Fert

#### Constructor
- `new Fert(metakernelPath)` - Initialize FERT with metakernel YAML file

#### Methods

##### getState(et, targetID, centerID, referenceID)
Get position vector of target relative to center.
- `et` - Ephemeris time in seconds after J2000
- `targetID` - SPICE ID of target body
- `centerID` - SPICE ID of center body  
- `referenceID` - SPICE ID of reference frame
- Returns: `[x, y, z]` position vector in km

##### getStateWithVelocity(et, targetID, centerID, referenceID)
Get position and velocity vectors.
- Returns: `{ position: [x, y, z], velocity: [vx, vy, vz] }`

##### getStateWithAcceleration(et, targetID, centerID, referenceID)
Get position, velocity, and acceleration vectors.
- Returns: `{ position: [x, y, z], velocity: [vx, vy, vz], acceleration: [ax, ay, az] }`

##### getStateLightTime(et, targetID, centerID, referenceID)
Get light-time corrected state.
- Returns: `{ position: [x, y, z], velocity: [vx, vy, vz], lightTime: seconds }`

##### getConstant(body, type)
Get physical or orbital constant.
- `body` - SPICE ID (number) or name (string) of body
- `type` - Type of constant (string)
- Returns: Constant value (number)

##### printSpkSummary()
Print summary of loaded SPK kernels to console.

##### printPckSummary()
Print summary of loaded PCK kernels to console.

### SPICE_IDS Object

Common SPICE body IDs and reference frames:

```javascript
const { SPICE_IDS } = require('fert-node');

// Bodies
SPICE_IDS.SUN                     // 10
SPICE_IDS.EARTH                   // 399
SPICE_IDS.MOON                    // 301
SPICE_IDS.MARS                    // 499
// ... and more

// Reference frames
SPICE_IDS.J2000                   // 1
SPICE_IDS.ECLIPJ2000             // 17
```

## Example

See `example.js` for a complete working example.

```bash
npm run test  # Runs example.js
```

## Error Handling

All methods throw JavaScript errors with descriptive messages when:
- Invalid parameters are provided
- Required ephemeris data is not available
- C++ library errors occur

Always wrap calls in try-catch blocks for robust error handling.

## License

Copyright (C) 2021 DART Lab - Politecnico di Milano. All rights reserved.