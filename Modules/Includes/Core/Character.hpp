#ifndef _MODULE_CORE_CHARACTER_HPP_
#define _MODULE_CORE_CHARACTER_HPP_

#include <Core/Need.hpp>
#include <Core/Emotion.hpp>
#include <Core/Environment.hpp>

#include <memory>
#include <vector>


/*
	Class that represents a character in the game.
*/

namespace Core
{
	struct PersonnalityModifier
	{
		//---------------------
		// Emotion modifier
		// Modify the influence of an emotion on the character.
		float FearFactor;
		float AngryFactor;
		float SadFactor;
		float JoyFactor;
		//---------------------

		//---------------------
		// Mutators
		float SadToAngryFactor; // Transfer between sad & angry. 0.1 will causes 10% of sadness to turns into anger.
		float FearToAngryFactor; // Transfer between angry & fear. 0.1 will cause 10% of fear to turns into anger.
		float LunaticFactor; // Modifier for emotions, 0 : Emotion doesn't change that much, 1 : Always changes.
		//---------------------
	};

	class Character
	{
		protected:

			std::vector<Need_Ptr> Needs;

			//-------------------------
			// Emotionnal state
			// Dominant emotion
			EmotionEnum Emotion;

			// Emotionnal state
			EmotionModifier FearState;
			EmotionModifier AngryState;
			EmotionModifier JoyState;
			EmotionModifier SadState;

			PersonnalityModifier Personnality;
			//-------------------------



			World_Ptr CurrentWorld;

		public:
			Character();

			//------------------------------------
			// Add a need to handle
			void AddNeed(Need* NewNeed);

			Need_Ptr& GetNeed(size_t Index);
			//------------------------------------

			//------------------------------------
			// Update the character
			// 	- Update each needs
			//  - Get dominant emotion (depending on personnality modifiers)
			//  - Mutate it (depending on personnality modifiers)
			virtual void Update();
			//------------------------------------

			//------------------------------------
			// This function is automatically called
			// when the character's emotion changes.
			virtual void OnEmotionChange(EmotionEnum NewEmotion) = 0;
			//------------------------------------

			//------------------------------------

			//------------------------------------
			// Getter / Setter
			void SetPersonnality(const PersonnalityModifier& PM)
			{ Personnality = PM; }

			const PersonnalityModifier& GetPersonnality() const
			{ return Personnality; }
			//------------------------------------
			virtual ~Character() {}
	};
	typedef std::shared_ptr<Character> Character_Ptr;
}

#endif
