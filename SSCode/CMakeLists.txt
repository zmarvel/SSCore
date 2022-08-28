

add_library(sscode STATIC
    SSAngle.cpp
    SSConstellation.cpp
    SSCoordinates.cpp
    SSEvent.cpp
    SSFeature.cpp
    SSHTM.cpp
    SSIdentifier.cpp
    SSImportGJ.cpp
    SSImportHIP.cpp
    SSImportMPC.cpp
    SSImportSKY2000.cpp
    SSJPLDEphemeris.cpp
    SSMatrix.cpp
    SSMoonEphemeris.cpp
    SSObject.cpp
    SSOrbit.cpp
    SSPlanet.cpp
    SSPSEphemeris.cpp
    SSStar.cpp
    SSTime.cpp
    SSTLE.cpp
    SSUtilities.cpp
    SSVector.cpp
    SSView.cpp
    VSOP2013/ELPMPP02.cpp
    VSOP2013/VSOP2013.cpp
    VSOP2013/VSOP2013p1.cpp
    VSOP2013/VSOP2013p2.cpp
    VSOP2013/VSOP2013p3.cpp
    VSOP2013/VSOP2013p4.cpp
    VSOP2013/VSOP2013p5.cpp
    VSOP2013/VSOP2013p6.cpp
    VSOP2013/VSOP2013p7.cpp
    VSOP2013/VSOP2013p8.cpp
    VSOP2013/VSOP2013p9.cpp
)
target_include_directories(sscode PUBLIC . VSOP2013)
