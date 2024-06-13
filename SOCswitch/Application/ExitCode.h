#pragma once

enum class ExitCode : int {
	Success = 0,
	Warnings = -1,
	Errors = -2,
	Fatal = -3
};
