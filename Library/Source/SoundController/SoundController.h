/**
 CSoundController
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "../DesignPatterns/SingletonTemplate.h"

// Include GLEW
#include <includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// Include string
#include <string>
// Include map storage
#include <map>
using namespace std;

// Include SoundInfo class; it stores the sound and other information
#include "SoundInfo.h"

enum SOUND_ID
{
	SOUND_START			= 0,
	SOUND_BELL			= 1,
	SOUND_EXPLOSION		= 2,
	SOUND_JUMP			= 3,
	SOUND_HIT			= 4,
	SOUND_SWING			= 5,
	SOUND_SWITCH		= 6,
	SOUND_GAME_COMPLETE = 7,
	SOUND_DEATH			= 8,
	SOUND_ONCLICK       = 9,
	SOUND_TELEPORTER    = 10,
	SOUND_POTION        = 11,
	SOUND_DOOR_OPEN     = 12,
	SOUND_DOOR_CLOSE	= 13,
	SOUND_CHEST			= 14,
	SOUND_TOTAL			   ,

	BGM_START			= 50,
	BGM_HENESYS				,
	BGM_TROUBLE             ,
	BGM_LEVEL1              ,
	BGM_WIN                 ,
	BGM_TOTAL			

};
class CSoundController : public CSingletonTemplate<CSoundController>
{
	friend CSingletonTemplate<CSoundController>;
public:
	// Initialise this class instance
	bool Init(void);

	// Load a sound
	bool LoadSound(string filename,
		const int ID,
		const bool bPreload = true,
		const bool bIsLooped = false,
		const bool bIsOverlappable = false,
		CSoundInfo::SOUNDTYPE eSoundType = CSoundInfo::SOUNDTYPE::_2D,
		vec3df vec3dfSoundPos = vec3df(0.0f, 0.0f, 0.0f));

	// Play a sound by its ID
	void PlaySoundByID(const int ID);
	void StopSoundByID(const int ID);

	// Increase Master volume
	bool MasterVolumeIncrease(void);
	// Decrease Master volume
	bool MasterVolumeDecrease(void);

	void SetMasterVolume(float volume);

	// Increase volume of a ISoundSource
	bool VolumeIncrease(const int ID);
	// Decrease volume of a ISoundSource
	bool VolumeDecrease(const int ID);

	void SetVolumeByID(const int ID, float volume);

	void UpdatePlayBackVolume(float volume);

	void StopPlayBack();

	bool isCurrentlyPlaying(std::string name);

	// For 3D sounds only
	// Set Listener position
	void SetListenerPosition(const float x, const float y, const float z);
	// Set Listener direction
	void SetListenerDirection(const float x, const float y, const float z);

	std::string GetNamebyID(int ID);
protected:
	// Constructor
	CSoundController(void);

	// Destructor
	virtual ~CSoundController(void);

	// Get an sound from this map
	CSoundInfo* GetSound(const int ID);
	// Remove an sound from this map
	bool RemoveSound(const int ID);
	// Get the number of sounds in this map
	int GetNumOfSounds(void) const;


	// The handler to the irrklang Sound Engine
	ISoundEngine* cSoundEngine;

	ISound* snd;
	// The map of all the entity created
	std::map<int, CSoundInfo*> soundMap;

	// For 3D sound only: Listener position
	vec3df vec3dfListenerPos;
	// For 3D sound only: Listender view direction
	vec3df vec3dfListenerDir;
};

