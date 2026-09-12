cmake_minimum_required(VERSION 3.30)
project(madronalib)
add_library(madronalib INTERFACE)
add_library(madronalib::madronalib ALIAS madronalib)
target_sources(madronalib PUBLIC
	FILE_SET HEADERS
	BASE_DIRS
		${CMAKE_CURRENT_SOURCE_DIR}/source
	FILES
		${CMAKE_CURRENT_SOURCE_DIR}/source/app/MLPlatform.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPBuffer.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPFilters.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPFunctional.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPGens.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPMath.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPMathNEON.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPMathSSE.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPOps.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPProjections.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPRatio.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPRouting.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPSample.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPScalarMath.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPScale.h
		${CMAKE_CURRENT_SOURCE_DIR}/source/DSP/MLDSPUtils.h
)
target_include_directories(madronalib INTERFACE
	$<INSTALL_INTERFACE:include/madronalib>
	$<INSTALL_INTERFACE:include/madronalib/app>
	$<INSTALL_INTERFACE:include/madronalib/DSP>
)
install(
	TARGETS madronalib
	EXPORT madronalib
	FILE_SET HEADERS
	DESTINATION include/madronalib
)
install(EXPORT madronalib
    FILE madronalibConfig.cmake
    NAMESPACE madronalib::
    DESTINATION lib/cmake/madronalib
)
