//  SSStar.cpp
//  SSCore
//
//  Created by Tim DeBenedictis on 3/15/20.
//  Copyright © 2020 Southern Stars. All rights reserved.

#include <map>

#include "SSDynamics.hpp"
#include "SSStar.hpp"

SSStar::SSStar ( SSObjectType type ) : SSObject ( type )
{
	_names = vector<string> ( 0 );
	_idents = vector<SSIdentifier> ( 0 );

	_parallax = 0.0;
	_radvel = HUGE_VAL;
    _position = _velocity = SSVector ( HUGE_VAL, HUGE_VAL, HUGE_VAL );
    _Vmag = HUGE_VAL;
    _Bmag = HUGE_VAL;
	
	_spectrum = "";
}

// Constructs single star with all fields except type code
// set to empty strings or infinity, signifying "unknown".

SSStar::SSStar ( void ) : SSStar ( kTypeStar )
{

}

// Constructs variable star with all fields except type code
// set to empty strings or infinity, signifying "unknown".

SSVariableStar::SSVariableStar ( void ) : SSStar ( kTypeVariableStar )
{
	_varType = "";
	_varMaxMag = HUGE_VAL;
	_varMinMag = HUGE_VAL;
	_varPeriod = HUGE_VAL;
	_varEpoch = HUGE_VAL;
}

// Constructs double star with all fields except type code
// set to empty strings or infinity, signifying "unknown".

SSDoubleStar::SSDoubleStar ( void ) : SSStar ( kTypeDoubleStar )
{
	_comps = "";
	_magDelta = HUGE_VAL;
	_sep = HUGE_VAL;
	_PA = HUGE_VAL;
	_PAyr = HUGE_VAL;
}

// Constructs double variable star with all fields except type code
// set to empty strings or infinity, signifying "unknown".

SSDoubleVariableStar::SSDoubleVariableStar ( void ) : SSDoubleStar(), SSVariableStar()
{
	_type = kTypeDoubleVariableStar;
}

// Returns this star's identifier in a specific catalog.
// If not present, returns null identifier (i.e. zero).

SSIdentifier SSStar::getIdentifier ( SSCatalog cat )
{
	for ( int i = 0; i < _idents.size(); i++ )
		if ( _idents[i].catalog() == cat )
			return _idents[i];
	
	return SSIdentifier();
}

void SSStar::computeEphemeris ( SSDynamics &dyn )
{
    if ( _parallax > 0.0 )
    {
        _direction = _position + _velocity * ( dyn.jde - SSTime::kJ2000 );
        _distance = _direction.magnitude();
        _direction /= _distance;
        _magnitude = _Vmag + 5.0 * log10 ( _distance * _parallax );
    }
    else
    {
        _direction = _position;
        _distance = HUGE_VAL;
        _magnitude = _Vmag;
    }
}

// Sets this star's spherical coordinates in the fundamental frame,
// i.e. the star's mean equatorial J2000 coordinates at epoch 2000.
// The star's RA (coords.lon) and Dec (coords.lat) are in radians.
// The star's distance in parsecs (coords.rad) may be infinite if unknown.

void SSStar::setFundamentalCoords ( SSSpherical coords )
{
	_parallax = isinf ( coords.rad ) ? 0.0 : 1.0 / coords.rad;

	if ( _parallax <= 0.0 || isinf ( coords.rad ) )
		coords.rad = 1.0;
	
	_position = coords.toVectorPosition();
}

// Sets this star's spherical coordinates and proper motion in the fundamental frame
// i.e. the star's mean equatorial J2000 coordinates and proper motion at epoch 2000.
// The star's RA (coords.lon) and Dec (coords.lat) are in radians.
// The stars proper motion in RA (motion.ra) and dec (motion.dec) are in radians per Julian year.
// The star's distance in parsecs (coords.rad) may be infinite if unknown.
// The star's radial velocity in parsecs per year (motion.rad) may be infinite if unknown.
// Mathematically, both coordinates and motion are required to compute the star's rectangular
// heliocentric position and motion; practically, if you have its motion you'll also have its position,
// so we pass them both here.  You can extract them separately (see below.)

void SSStar::setFundamentalMotion ( SSSpherical coords, SSSpherical motion )
{
	_parallax = isinf ( coords.rad ) ? 0.0 : 1.0 / coords.rad;
	_radvel = motion.rad;

	if ( _parallax <= 0.0 )
	{
		coords.rad = 1.0;
		motion.rad = 0.0;
	}
	
	if ( isinf ( motion.rad ) )
		motion.rad = 0.0;
	
	_position = coords.toVectorPosition();
	_velocity = coords.toVectorVelocity ( motion );
}

// Returns this star's heliocentric spherical coordinates in the fundamental
// J2000 mean equatorial frame at epoch J2000.  The star's RA (coords.lon)
// and Dec (coords.lat) are in radians.  Its distance (coords.rad) is in
// parsecs and will be infinite if unknown.

SSSpherical SSStar::getFundamentalCoords ( void )
{
	SSSpherical coords = _position.toSpherical();
	coords.rad = isinf ( _parallax ) || _parallax == 0.0 ? HUGE_VAL : 1.0 / _parallax;
	return coords;
}

// Returns this star's heliocentric proper motion in the fundamental J2000
// mean equatorial frame at epoch J2000.  The proper motion in RA (motion.lon)
// and Dec (motion.lat) are both in radians per year.  Its radial velocity
// (motion.rad) is in parsecs per year and will be infinite if unknown.

SSSpherical SSStar::getFundamentalMotion ( void )
{
	SSSpherical motion = _position.toSphericalVelocity ( _velocity );
	motion.rad = _radvel;
	return motion;
}

typedef map<SSObjectType,string> SSTypeStringMap;

SSTypeStringMap _typeStrings =
{
	{ kTypeStar, "SS" },
	{ kTypeDoubleStar, "DS" },
	{ kTypeVariableStar, "VS" },
	{ kTypeDoubleVariableStar, "DV" },
	{ kTypeOpenCluster, "OC" },
	{ kTypeGlobularCluster, "GC" },
	{ kTypeBrightNebula, "BN" },
	{ kTypeDarkNebula, "DN" },
	{ kTypePlanetaryNebula, "PN" },
	{ kTypeGalaxy, "GX" },
	{ kTypePlanet, "PL" },
	{ kTypeMoon, "MN" },
	{ kTypeAsteroid, "MP" },
	{ kTypeComet, "CM" },
	{ kTypeSatellite, "SA" },
	{ kTypeSpacecraft, "SC" }
};

// Returns CSV string from base data (excluding names and identifiers).

string SSStar::toCSV1 ( void )
{
	string csv = "";
	
	SSSpherical pos = _position.toSpherical();
	SSSpherical vel = _position.toSphericalVelocity( _velocity );
	
	SSHourMinSec ra = pos.lon;
	SSDegMinSec dec = pos.lat;
	
	csv += _typeStrings[ _type ] + ",";
	
	csv += ra.toString() + ",";
	csv += dec.toString() + ",";
	
	csv += isnan ( vel.lon ) ? "        ," : format ( "%+8.5f,", ( vel.lon / 15.0 ).toArcsec() );
	csv += isnan ( vel.lat ) ? "        ," : format ( "%+7.4f,", vel.lat.toArcsec() );
	
	csv += isinf ( _Vmag ) ? "      ," : format ( "%+6.2f,", _Vmag );
	csv += isinf ( _Bmag ) ? "      ," : format ( "%+6.2f,", _Bmag );
	
	csv += isinf ( _parallax ) ? "      ," : format ( "%6.4f,", _parallax );
	csv += isinf ( _radvel )   ? "      ," : format ( "%+6.1f,", _radvel * SSDynamics::kLightKmPerSec );
	
	csv += _spectrum + ",";
	
	return csv;
}

// Returns CSV string from names and identifiers (excluding base data).

string SSStar::toCSV2 ( void )
{
	string csv = "";
	
	for ( int i = 0; i < _names.size(); i++ )
		csv += _names[i] + ",";

	for ( int i = 0; i < _idents.size(); i++ )
		csv += _idents[i].toString() + ",";
	
	return csv;
}

// Returns CSV string including base star data plus names and identifiers.

string SSStar::toCSV ( void )
{
	return toCSV1() + toCSV2();
}

// Returns CSV string from double-star data (but not SStar base class).

string SSDoubleStar::toCSVD ( void )
{
	string csv = "";
	
	csv += _comps + ",";
	csv += isinf ( _magDelta ) ? "      ," : format ( "%+6.2f", _magDelta );
	csv += isinf ( _sep ) ? "      ," : format ( "%6.1f", _sep * SSAngle::kArcsecPerRad );
	csv += isinf ( _PA ) ? "     ," : format ( "%5.1f", _PA * SSAngle::kDegPerRad );
	csv += isinf ( _PAyr ) ? "       ," : format ( "%7.2f", _PAyr );

	return csv;
}

// Returns CSV string including base star data, double-star data,
// plus names and identifiers. Overrides SSStar::toCSV().

string SSDoubleStar::toCSV ( void )
{
	return toCSV1() + toCSVD() + toCSV2();
}

// Returns CSV string from variable-star data (but not SStar base class).

string SSVariableStar::toCSVV ( void )
{
	string csv = "";
	
	csv += _varType + ",";
	csv += isinf ( _varMinMag ) ? "      ," : format ( "%+6.2f", _varMinMag );
	csv += isinf ( _varMaxMag ) ? "      ," : format ( "%+6.2f", _varMaxMag );
	csv += isinf ( _varPeriod ) ? "       ," : format ( "%7.2f", _varPeriod );
	csv += isinf ( _varEpoch )  ? "         ," : format ( "%9.2f", _varEpoch );

	return csv;
}

// Returns CSV string including base star data, variable-star data, plus names and identifiers.
// Overrides SSStar::toCSV().

string SSVariableStar::toCSV ( void )
{
	return toCSV1() + toCSVV() + toCSV2();
}

// Returns CSV string including base star data, double-star data, variable-star data,
// plus names and identifiers.  Overrides SSStar::toCSV().

string SSDoubleVariableStar::toCSV ( void )
{
	return toCSV1() + toCSVD() + toCSVV() + toCSV2();
}

// Downcasts generic SSObject pointer to SSStar pointer.
// Returns nullptr if SSObject is not an SSStar!

SSStarPtr SSGetStarPtr ( SSObjectPtr ptr )
{
	return dynamic_cast<SSStarPtr> ( ptr.get() );
}

// Downcasts generic SSObject pointer to SSDoubleStar pointer.
// Returns nullptr if SSObject is not an SSDoubleStar or SSDoubleVariableStar!

SSDoubleStarPtr SSGetDoubleStarPtr ( SSObjectPtr ptr )
{
	return dynamic_cast<SSDoubleStarPtr> ( ptr.get() );
}

// Downcasts generic SSObject pointer to SSVariableStar pointer.
// Returns nullptr if SSObject is not an SSVariableStar or SSDoubleVariableStar!

SSVariableStarPtr SSGetVariableStarPtr ( SSObjectPtr ptr )
{
	return dynamic_cast<SSVariableStarPtr> ( ptr.get() );
}
