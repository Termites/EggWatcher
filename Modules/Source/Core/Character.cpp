#include <Core/Character.hpp>

Core::Character::Character()
{


	//----------------------------------
	// Setup default personnality
	Personnality.FearFactor = 1.0f;
	Personnality.AngryFactor = 1.0f;
	Personnality.SadFactor = 1.0f;
	Personnality.JoyFactor = 1.0f;

	Personnality.SadToAngryFactor = 0.1f;
	Personnality.FearToAngryFactor = 0.02f;
	Personnality.LunaticFactor = 0.5f;
	//----------------------------------

	//----------------------------------
	// Setup default emotional state
	Emotion = Core::EmotionEnum::Neutral;

	FearState.first = Core::EmotionEnum::Fear;
	FearState.second = 0.0f;

	AngryState.first = Core::EmotionEnum::Angry;
	AngryState.second = 0.0f;

	JoyState.first = Core::EmotionEnum::Happy;
	JoyState.second = 0.0f;

	SadState.first = Core::EmotionEnum::Sad;
	SadState.second = 0.0f;
	//----------------------------------
}

void Core::Character::AddNeed(Core::Need* NewNeed)
{
	Needs.push_back(Need_Ptr(NewNeed));
}

Core::Need_Ptr& Core::Character::GetNeed(size_t Index)
{
	// todo : exception

	return Needs [ Index ];
}

void Core::Character::Update()
{
	//==========================================
	// Update all needs
	// - Call update function
	// - Update emotional state, depending on need's return value
	for (auto& N : Needs)
	{
		Core::EmotionModifier EM = N->Update();

		switch (EM.first)
		{
			case EmotionEnum::Fear:
				FearState.second += EM.second * Personnality.LunaticFactor;
			break;

			case EmotionEnum::Angry:
				AngryState.second += EM.second * Personnality.LunaticFactor;
			break;

			case EmotionEnum::Sad:
				SadState.second += EM.second * Personnality.LunaticFactor;
			break;

			case EmotionEnum::Neutral:
				// Special case, skip it ?
			break;

			case EmotionEnum::Happy:
				JoyState.second += EM.second * Personnality.LunaticFactor;
			break;
		}
	}
	//==========================================

	//==========================================
	// Mutate feelings
	float TransferValue = Personnality.SadToAngryFactor * SadState.second;

	SadState.second -= TransferValue * (1.0-Personnality.SadFactor);
	AngryState.second += TransferValue * Personnality.FearFactor;

	TransferValue = Personnality.FearToAngryFactor * FearState.second;

	FearState.second -= TransferValue * (1.0-Personnality.FearFactor);
	AngryState.second += TransferValue * Personnality.AngryFactor;
	//==========================================

	//==========================================
	// Get dominant feeling
	Core::EmotionModifier NewEmotion = Core::EmotionModifier(Core::EmotionEnum::Neutral,0.0f);

	Core::EmotionModifier EmotStats [4];
	EmotStats[0].first = Core::EmotionEnum::Fear;
	EmotStats[0].second = FearState.second * Personnality.FearFactor;

	EmotStats[1].first = Core::EmotionEnum::Angry;
	EmotStats[1].second = AngryState.second * Personnality.AngryFactor;

	EmotStats[2].first = Core::EmotionEnum::Sad;
	EmotStats[2].second = SadState.second * Personnality.SadFactor;

	EmotStats[3].first = Core::EmotionEnum::Happy;
	EmotStats[3].second = JoyState.second * Personnality.JoyFactor;

    float Average = 0.0f;
	for (auto& E : EmotStats)
	{
	    Average += E.second;
		if (E.second > NewEmotion.second)
		{
			NewEmotion = E;
		}
	}

	Average /= 4;

	// Check emotional change, call function in case of changement.
	if (NewEmotion.first != Emotion &&  NewEmotion.second > Average)
	{
	    OnEmotionChange(NewEmotion.first);
		Emotion = NewEmotion.first;
	}
	//==========================================
}
