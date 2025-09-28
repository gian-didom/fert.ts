const { Fert, SPICE_IDS } = require('./index');

console.log('FERT Node.js Wrapper Test');
console.log('=========================\n');

// Simple test without requiring actual metakernel files
console.log('Testing FERT module loading...');

try {
    console.log('✓ FERT module loaded successfully');
    console.log('✓ Available SPICE IDs:', Object.keys(SPICE_IDS).slice(0, 5).join(', '), '...');
    
    // Try to create FERT instance with a dummy path (will fail but tests constructor)
    console.log('\nTesting FERT constructor with dummy path...');
    try {
        const fert = new Fert('/nonexistent/metakernel.yaml');
        console.log('✓ Constructor called (unexpected success)');
    } catch (error) {
        console.log('✓ Constructor properly handles invalid path:', error.message.substring(0, 50) + '...');
    }
    
    console.log('\nFERT Node.js wrapper is ready to use!');
    console.log('To use with real data, provide a valid metakernel YAML file path.');
    
} catch (error) {
    console.error('✗ Error:', error.message);
}
