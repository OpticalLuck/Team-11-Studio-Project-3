#include "Command.h"

Command::Command() 
	: m_Command(COMMAND_NONE)
{
}

Command::Command(COMMANDS command)
	: m_Command(command)
{
}

Command::~Command()
{
}

void Command::RunCommand()
{
	switch (m_Command)
	{
	case COMMAND_EXAMPLE_OPEN_DOOR_1:
		// open door
		break;
	default:
		break;
	}
}

void Command::SetCommand(COMMANDS command)
{
	m_Command = command;
}
