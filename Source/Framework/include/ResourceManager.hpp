#pragma once

enum class ResourceType
{
	IN_RAM,	///< resource stored in ram (allocated or static)
	EXTERNAL ///< external resource which should be uploaded from hdd/netwrork
};