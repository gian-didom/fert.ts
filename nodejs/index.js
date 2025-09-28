const bindings = require('bindings');
const fertAddon = bindings('fert-node');

/**
 * Fast Ephemeris Retrieval Tool (FERT) - Node.js wrapper
 * 
 * This module provides a Node.js interface to the FERT C++ library
 * for fast ephemeris retrieval and orbital mechanics calculations.
 */

class Fert {
    /**
     * Create a new FERT instance
     * @param {string} metakernelPath - Path to the metakernel YAML file
     */
    constructor(metakernelPath) {
        this._fert = new fertAddon.Fert(metakernelPath);
    }

    /**
     * Get position vector of a target relative to a center
     * @param {number} et - Ephemeris time in seconds after J2000
     * @param {number} targetID - SPICE ID of the target body
     * @param {number} centerID - SPICE ID of the center body
     * @param {number} referenceID - SPICE ID of the reference frame
     * @returns {number[]} Position vector [x, y, z] in kilometers
     */
    getState(et, targetID, centerID, referenceID) {
        return this._fert.getState(et, targetID, centerID, referenceID);
    }

    /**
     * Get position and velocity vectors of a target relative to a center
     * @param {number} et - Ephemeris time in seconds after J2000
     * @param {number} targetID - SPICE ID of the target body
     * @param {number} centerID - SPICE ID of the center body
     * @param {number} referenceID - SPICE ID of the reference frame
     * @returns {Object} Object with position and velocity arrays
     * @returns {number[]} returns.position - Position vector [x, y, z] in kilometers
     * @returns {number[]} returns.velocity - Velocity vector [vx, vy, vz] in km/s
     */
    getStateWithVelocity(et, targetID, centerID, referenceID) {
        return this._fert.getStateWithVelocity(et, targetID, centerID, referenceID);
    }

    /**
     * Print summary of loaded SPK kernels to console
     */
    printSpkSummary() {
        this._fert.printSpkSummary();
    }

    /**
     * Print summary of loaded PCK kernels to console
     */
    printPckSummary() {
        this._fert.printPckSummary();
    }
}

// Common SPICE body IDs for convenience
const SPICE_IDS = {
    // Solar system barycenter and Sun
    SOLAR_SYSTEM_BARYCENTER: 0,
    SUN: 10,
    
    // Planets
    MERCURY: 199,
    VENUS: 299,
    EARTH: 399,
    MARS: 499,
    JUPITER: 599,
    SATURN: 699,
    URANUS: 799,
    NEPTUNE: 899,
    PLUTO: 999,
    
    // Earth-Moon system
    EARTH_BARYCENTER: 3,
    MOON: 301,
    
    // Common reference frames
    J2000: 1,
    ECLIPJ2000: 17,
    GALACTIC: 18
};

module.exports = {
    Fert,
    SPICE_IDS
};