#include "plibsys.h"
#include "BaseLibrary.h"
#include "models.h"
#include "PingService.h"

/**
 * @brief Initializes the base library
 * 
 */
void InitializeBaseLibrary() {
	p_libsys_init();
	register_models();
	PingService::getInstance().start();
}

/**
 * @brief Shuts down the base library, de-allocating all resources
 * 
 */
void ShutdownBaseLibrary()
{
	PingService::getInstance().stop();
	p_libsys_shutdown();
	TypeInfo::NATIVE_TYPES.clear();
	TypeInfo::TYPES.clear();
}