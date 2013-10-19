#ifndef _MODULE_CORE_EMOTION_HPP_
#define _MODULE_CORE_EMOTION_HPP_

#include <string>
#include <utility>
/*
	Emotional state
*/

namespace Core
{
	enum class EmotionEnum
	{
		Fear,
		Angry,
		Sad,
		Neutral,
		Happy

	};

	// Type that is used to modify character's emotion
	// From all the needs the character have, the most
	// requested is kept.
	typedef std::pair<EmotionEnum, float> EmotionModifier;

	inline std::string EmotionToString(EmotionEnum EE)
	{
		switch (EE)
		{
			case EmotionEnum::Fear:
				return "Fear";
			break;

			case EmotionEnum::Angry:
				return "Angry";
			break;

			case EmotionEnum::Sad:
				return "Sad";
			break;

			case EmotionEnum::Neutral:
				return "Neutral";
			break;

			case EmotionEnum::Happy:
				return "Happy";
			break;

			default:
                return "None";
            break;
		}
	}
}

#endif
