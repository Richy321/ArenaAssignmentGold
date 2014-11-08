#pragma once

namespace Arena
{
	class SoundManager
	{

	private:
		enum
		{ 
			numSounds = 8
		};
		
		unsigned int curSource = 0;
		octet::ALuint sources[numSounds];


	public:
		octet::ALuint futuristicGunshot;
		octet::ALuint pickupSound;
		octet::ALuint enemyExplode;

		SoundManager()
		{
			futuristicGunshot = octet::resource_dict::get_sound_handle(octet::AL_FORMAT_MONO16, "src/examples/arena/assets/sounds/futuristicGunshot.wav");
			pickupSound = octet::resource_dict::get_sound_handle(octet::AL_FORMAT_MONO16, "src/examples/arena/assets/sounds/pickupSound.wav");
			enemyExplode = octet::resource_dict::get_sound_handle(octet::AL_FORMAT_MONO16, "src/examples/arena/assets/sounds/explosion_medium.wav");
			
			octet::alGenSources(numSounds, sources);
		}

		virtual ~SoundManager()
		{
		}

		octet::ALuint getSoundSource() { return sources[curSource++ % numSounds]; }

		void Play(octet::ALuint sound)
		{
			octet::ALuint source = getSoundSource();
			octet::alSourcei(source, octet::AL_BUFFER, sound);
			octet::alSourcePlay(source);
		}
	};
}
