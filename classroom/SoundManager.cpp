#include "SoundManager.h"

//ALC specifics
ALCcontext *context;
ALCdevice *device;

// File reading stuff 

FILE *fp = NULL;

char type[4];
DWORD size, chunkSize;
short formatType, channels;
DWORD sampleRate, avgBytesPerSec;
short bytesPerSample, bitsPerSample;
DWORD dataSize;

// Spatial sound attributes

SoundManager::SoundManager() {
	std::cout << "SoundManager is alive!" << std::endl;
	
	SourcePos[0] = 0.0f; SourcePos[1] = 0.0f; SourcePos[2] = 0.0f;
	SourceVel[0] = 0.0f; SourceVel[1] = 0.0f; SourceVel[2] = 0.0f;
	ListenerPos[0] = 0.0f; ListenerPos[1] = 0.0f; ListenerPos[2] = 0.0f;
	ListenerVel[0] = 0.0f; ListenerVel[1] = 0.0f; ListenerVel[2] = 0.0f;
	
	ListenerOri[0] = 0.0f; ListenerOri[1] = 0.0f; ListenerOri[2] = -1.0f;
	ListenerOri[3] = 0.0f; ListenerOri[4] = 1.0f; ListenerOri[5] = 0.0f;


}

SoundManager::~SoundManager() {
	stopMusic();
	// Destroy the context 
	alcMakeContextCurrent(NULL);
	if (context)
		alcDestroyContext(context);

	// Destroy the device 
	if (device)
		alcCloseDevice(device);
}

void SoundManager::init(float _bg, float _sound) {
	std::cout << "SoundManager initiated!" << std::endl;

	// Set parameters after arguments
	_bgVolume = _bg;
	_soundVolume = _sound;

	// Init OpenAL
	device = alcOpenDevice(NULL);
	if (!device) return;
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	if (!context) return;


	/*alGenSources(1, &gameMusicSource);*/
	alGenSources(1, &inGameSource);
	alGenSources(1, &inGameLowHPSource);
	alGenSources(1, &preGameSource);
	alGenSources(1, &gameOverSource);
	alGenSources(1, &explosionSource);
	alGenSources(1, &menuMusicSource);
	alGenSources(1, &laserSource);
	alGenSources(1, &laserHitSource);
	alGenSources(1, &powerupSource);

	/*setSource(&gameMusicSource, "sounds/gameOver2.wav");*/
	setSource(&inGameSource, "sounds/ingame_strings.wav");
	setSource(&inGameLowHPSource, "sounds/ingame_lowhp.wav");
	setSource(&preGameSource, "sounds/pregame2.wav");
	setSource(&gameOverSource, "sounds/gameOver2.wav");
	setSource(&explosionSource, "sounds/explosion2.wav");
	setSource(&menuMusicSource, "sounds/menu2.wav");
	setSource(&laserSource, "sounds/laser.wav");
	setSource(&laserHitSource, "sounds/laser_hit.wav");
	setSource(&powerupSource, "sounds/powerup.wav");

}

//! Plays the score parameter at given position vec3.
void SoundManager::play(std::string score, osg::Vec3f position) {

	if (_MUTED)
		return;

	// Convert position glm::vector to ALfloat::array for playback 
	ALfloat soundPosition[3];
	soundPosition[0] = position.x();
	soundPosition[1] = position.y();
	soundPosition[2] = position.z();

	//std::cout << soundPosition[0] << " " << position.y() << " " << position.z() << "score = " << score << std::endl;


	if (score == "mainMenu_music") {
		stopMusic();
		alSourcef(menuMusicSource, AL_GAIN, _bgVolume);
		alSourcefv(menuMusicSource, AL_POSITION, soundPosition);
		alSourcei(menuMusicSource, AL_LOOPING, AL_TRUE);
		alSourcePlay(menuMusicSource);
		currentBackgroundScore = &menuMusicSource;
		bgIsPlaying = true;
	}
	if (score == "preGame_music") {
		std::cout << "playing preGame_music!" << std::endl;
		stopMusic();
		alSourcef(preGameSource, AL_GAIN, _bgVolume);
		alSourcefv(preGameSource, AL_POSITION, soundPosition);
		alSourcePlay(preGameSource);
		currentBackgroundScore = &preGameSource;
		bgIsPlaying = true;
	}
	if (score == "inGame_music") {
		stopMusic();
		alSourcef(inGameSource, AL_GAIN, _bgVolume);
		alSourcefv(inGameSource, AL_POSITION, soundPosition);
		alSourcei(inGameSource, AL_LOOPING, AL_TRUE);
		alSourcePlay(inGameSource);
		currentBackgroundScore = &inGameSource;
		bgIsPlaying = true;
	}
	if (score == "lowHP_music") {
		stopMusic();
		alSourcef(inGameLowHPSource, AL_GAIN, _bgVolume);
		alSourcefv(inGameLowHPSource, AL_POSITION, soundPosition);
		alSourcei(inGameLowHPSource, AL_LOOPING, AL_TRUE);
		alSourcePlay(inGameLowHPSource);
		currentBackgroundScore = &inGameLowHPSource;
		bgIsPlaying = true;
	}

	if (score == "gameOver") {
		alSourcef(gameOverSource, AL_GAIN, _soundVolume);
		alSourcefv(gameOverSource, AL_POSITION, soundPosition);
		alSourcePlay(gameOverSource);
	}
	if (score == "laser") {
		alSourcef(laserSource, AL_GAIN, _soundVolume);
		alSourcefv(laserSource, AL_POSITION, soundPosition);
		alSourcePlay(laserSource);
	}
	if (score == "explosion") {
		alSourcef(explosionSource, AL_GAIN, _soundVolume);
		alSourcefv(explosionSource, AL_POSITION, soundPosition);
		alSourcePlay(explosionSource);
	}
	if (score == "powerup") {
		alSourcef(powerupSource, AL_GAIN, _soundVolume);
		alSourcefv(powerupSource, AL_POSITION, soundPosition);
		alSourcePlay(powerupSource);
	}
	if (score == "laserHit") {
		alSourcef(laserHitSource, AL_GAIN, _soundVolume);
		alSourcefv(laserHitSource, AL_POSITION, soundPosition);
		alSourcePlay(laserHitSource);
	}
}

void SoundManager::stopMusic() {
	if (bgIsPlaying) {
		alSourceStop(*currentBackgroundScore);
		bgIsPlaying = false;
	}
	else {
		// nothing playing, do nothing;
	}
}

//! Takes parameter URL, loads file and sets up sound for source parameter
void SoundManager::setSource(ALuint *source, char *url) {
	std::cout << "Attempting to set '" << url << "' as source for sound!" << std::endl;

	// Load the current url
	fp = fopen(url, "rb");

	// Check riff type, wave type and fmt
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') return;

	fread(&size, sizeof(DWORD), 1, fp);
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') return;

	fread(type, sizeof(char), 4, fp); 
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') return;

	// read additional info about file.

	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	// make sure file has been read
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a') return;
	fread(&dataSize, sizeof(DWORD), 1, fp);

	// allocate memory for sound data and read the data
	unsigned char* buf = new unsigned char[dataSize];
	fread(buf, sizeof(BYTE), dataSize, fp);

	// Sound generation

	ALuint buffer;
	ALuint frequency = sampleRate;
	ALenum format = 0;

	alGenBuffers(1, &buffer);
	alGenSources(1, source);

	// let OpenAL know the format of the sound
	format = setSoundFormat(bitsPerSample);

	alBufferData(buffer, format, buf, dataSize, frequency);

	// Listener
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);

	alSourcei(*source, AL_BUFFER, buffer);
	alSourcef(*source, AL_PITCH, 1.0f);
	alSourcef(*source, AL_GAIN, 1.0f);
	alSourcefv(*source, AL_POSITION, SourcePos);
	alSourcefv(*source, AL_VELOCITY, SourceVel);
	alSourcei(*source, AL_LOOPING, AL_FALSE);

	std::cout << "Success: '" << url << "' was loaded." << std::endl;
}

ALenum SoundManager::setSoundFormat(short bitsPerSample) {
	if (bitsPerSample == 8) {
		if (channels == 1)
			return AL_FORMAT_MONO8;
		else if (channels == 2)
			return AL_FORMAT_STEREO8;
	}
	else if (bitsPerSample == 16) {
		if (channels == 1)
			return AL_FORMAT_MONO16;
		else if (channels == 2)
			return AL_FORMAT_STEREO16;
	}
}

void SoundManager::muteAll() {
	if (!_MUTED) {
		stopMusic();
		_MUTED = true;
	}
	else {
		alSourcePlay(*currentBackgroundScore);
		_MUTED = false;
	}
}