const { Fert, SPICE_IDS } = require('./index');
const path = require('path');

// Example usage of the FERT Node.js wrapper
console.log('FERT Node.js Wrapper Example');
console.log('============================\n');

// You'll need to provide a path to your metakernel YAML file
// This is just an example - replace with your actual metakernel path
const metakernelPath = path.join(__dirname, 'data', 'metaKernel.yml');

try {
    // Initialize FERT with metakernel
    console.log(`Initializing FERT with metakernel: ${metakernelPath}`);
    const fert = new Fert(metakernelPath);
    
    console.log('FERT initialized successfully!\n');
    
    // Print loaded kernel summaries
    console.log('SPK Kernel Summary:');
    fert.printSpkSummary();
    
    console.log('\nPCK Kernel Summary:');
    fert.printPckSummary();
    
    // Example: Get Earth's position relative to the Solar System Barycenter
    const et = 6.696e+08; // J2000 epoch (2000-01-01 12:00:00 TDB)
    
    console.log(`\nExamples at J2000 epoch (ET = ${et} seconds):`);
    console.log('===========================================');
    
    // Get Earth's position only
    try {
        const earthPosition = fert.getState(et, SPICE_IDS.EARTH, SPICE_IDS.EARTH_MOON_BARYCENTER, SPICE_IDS.J2000);
        console.log(`Earth position relative to EMB: [${earthPosition[0].toFixed(3)}, ${earthPosition[1].toFixed(3)}, ${earthPosition[2].toFixed(3)}] km`);
    } catch (error) {
        console.log(`Error getting Earth position: ${error.message}`);
    }
    
    // Get Earth's position and velocity
    try {
        const earthState = fert.getStateWithVelocity(et, SPICE_IDS.EARTH, SPICE_IDS.SOLAR_SYSTEM_BARYCENTER, SPICE_IDS.J2000);
        console.log(`Earth position: [${earthState.position[0].toFixed(3)}, ${earthState.position[1].toFixed(3)}, ${earthState.position[2].toFixed(3)}] km`);
        console.log(`Earth velocity: [${earthState.velocity[0].toFixed(6)}, ${earthState.velocity[1].toFixed(6)}, ${earthState.velocity[2].toFixed(6)}] km/s`);
    } catch (error) {
        console.log(`Error getting Earth state: ${error.message}`);
    }
    
    // Get Moon's position relative to Earth with light-time correction
    try {
        const moonState = fert.getStateLightTime(et, SPICE_IDS.MOON, SPICE_IDS.EARTH, SPICE_IDS.J2000);
        console.log(`Moon position (LT corrected): [${moonState.position[0].toFixed(3)}, ${moonState.position[1].toFixed(3)}, ${moonState.position[2].toFixed(3)}] km`);
        console.log(`Moon velocity (LT corrected): [${moonState.velocity[0].toFixed(6)}, ${moonState.velocity[1].toFixed(6)}, ${moonState.velocity[2].toFixed(6)}] km/s`);
        console.log(`Light time: ${moonState.lightTime.toFixed(6)} seconds`);
    } catch (error) {
        console.log(`Error getting Moon state with light time: ${error.message}`);
    }
    
    // Get a physical constant
    try {
        const earthGM = fert.getConstant(SPICE_IDS.EARTH, 'GM');
        console.log(`Earth GM: ${earthGM} km³/s²`);
    } catch (error) {
        console.log(`Error getting Earth GM: ${error.message}`);
    }
    
    console.log('\nExample completed successfully!');
    
} catch (error) {
    console.error('Error initializing FERT:', error.message);
    console.error('Make sure you have a valid metakernel YAML file and required SPK/PCK files.');
}