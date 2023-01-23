#include "realsim/core/application.h"


namespace rsim::core
{
	application::application(cmd_line_args args)
    : m_StartArgs(args)
	{

	}

	return_code application::Run()
	{
		volatile int loop = 1;
		while (loop);
		return REALSIM_MAIN_SUCCESS;
	}
}
