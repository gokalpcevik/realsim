#pragma once
#include <cstdint>
#include <iostream>
#include "realsim/realsim.h"

#define REALSIM_MAIN_SUCCESS 0
#define REALSIM_MAIN_FAIL (-1)

namespace rsim::core
{
	class application
	{
        struct cmd_line_args
        {
            cmd_line_args() = default;
            int Count;
            char** Args;
        };

	public:
		explicit application(cmd_line_args args);

		virtual return_code Run();
    protected:
        cmd_line_args m_StartArgs{};
    };
}