#ifndef _MODULE_CORE_ENVIRONMENT_HPP_
#define _MODULE_CORE_ENVIRONMENT_HPP_

#include <memory>

/*
	Environment related classes & structures
*/

namespace Core
{
	//========================
	// Environment Parameter :
	// Represents environment's properties
	// independently of weather conditions.
	struct EnvironmentParameter
	{
		float Humidity;
		float Temperature;
	};
	//========================

	//========================
	// Enumeration for the weathers
	enum class WeatherType
	{
		Rain,
		Snow,
		Cloud,
		Haze
	};

	// Pair that assign to a weather it's value
	// 0.0 = None -> 1.0 = Extreme
	typedef std::pair<WeatherType, float> WeatherValue;
	//========================

	//========================
	// World :
	// Class that handle the current
	// state of the world.
	// Weather + Env. props + Wind
	// Can be inherited.
	class World
	{
		protected:
			EnvironmentParameter Env;

			//-----------------------------
			//Weather
			WeatherValue Rain;
			WeatherValue Snow;
			WeatherValue Cloud;
			WeatherValue Haze;

			float WindForce;
			float WindAngle;
			//-----------------------------

		public:
			World()
			{
				//----------------------------------------
				// Setup default values for weather
				Rain = WeatherValue(WeatherType::Rain, 0.0f);
				Snow = WeatherValue(WeatherType::Snow, 0.0f);
				Cloud = WeatherValue(WeatherType::Cloud, 0.0f);
				Haze = WeatherValue(WeatherType::Haze, 0.0f);
				//----------------------------------------

				//----------------------------------------
				// Default values for wind
				WindForce = 0.0f;
				WindAngle = 0;
				//----------------------------------------

				//----------------------------------------
				// Default values for environment
				Env.Humidity = 0.25;
				Env.Temperature = 25;
				//----------------------------------------

			}

			// Update world (changes weather, wind, environment values, etc,etc)
			virtual void Update() {}

			//---------------------------------------------
			// Getters / Setters
			//
			inline float GetWeather(WeatherType WT)
			{
				switch (WT)
				{
					case WeatherType::Rain:
						return Rain.second;
					break;

					case WeatherType::Snow:
						return Snow.second;
					break;

					case WeatherType::Cloud:
						return Cloud.second;
					break;

					case WeatherType::Haze:
						return Haze.second;
					break;
				}
			}
			
			void SetWeather(WeatherType WT, float Value)
			{
				switch (WT)
				{
					case WeatherType::Rain:
						Rain.second = Value;
					break;

					case WeatherType::Snow:
						Snow.second = Value;
					break;

					case WeatherType::Cloud:
						Cloud.second = Value;
					break;

					case WeatherType::Haze:
						Haze.second = Value;
					break;
				}			
			}

			EnvironmentParameter& GetEnvironmentParameters()
			{ return Env; }

			const EnvironmentParameter& GetEnvironmentParameters() const
			{ return Env; }
			//---------------------------------------------

			virtual ~World()
			{}
	};
	typedef std::shared_ptr<World> World_Ptr;
	//========================
}

#endif
