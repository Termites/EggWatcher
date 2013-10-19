#ifndef _MODULE_CORE_NEED_HPP_
#define _MODULE_CORE_NEED_HPP_

#include <memory>
#include <Core/Emotion.hpp>
#include <Core/Environment.hpp>

namespace Core
{
	//================================================
	// Class that handle character's needs.
	// Must be inherited.
	class Need
	{
		protected:
			float Factor;
			
			World_Ptr World;
			
		public:
			Need()
			{ Factor = 0.0f; }
			
			void SetWorld(const World_Ptr& World) 
			{ this->World = World; }
			
			//---------------------------------------
			// Update function : 
			// Once factor reached 1, it automatically
			// triggers an event.
			// Returns EmotionModifier
			virtual EmotionModifier Update() = 0;
			
			virtual ~Need() {}
	};

	typedef std::shared_ptr<Need> Need_Ptr;
	//================================================

}
#endif