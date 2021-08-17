#pragma once

#include <string>

// Enumerators for commands for faster switch case so i dont have to use if checks for strings waste of call stacks
enum COMMANDS
{
	COMMAND_NONE,
	COMMAND_EXAMPLE_OPEN_DOOR_1,
	COMMAND_TOTAL,
};

class Command
{
public:
	Command();
	Command(COMMANDS command);
	~Command();

	void RunCommand();
	void SetCommand(COMMANDS command);
protected:
	COMMANDS m_Command;
};

