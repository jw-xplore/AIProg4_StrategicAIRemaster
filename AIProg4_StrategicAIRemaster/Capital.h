#pragma once
/*
Capital represents exchangeable "gameplay" resources like trees and iron
Capital types and operations are defined here
*/

namespace Capital
{
	enum ECapitalType
	{
		None = -1,
		Tree,
		Coal,
		IronOre,
		IronBar,
		Sword,
		ECapitalTypeCount
	};

	// Tracks amounts of the resources for each type
	// Can be used for stored or defining how resources cost - e.g. how much wood and iron will cost to build a smelter
	struct CapitalAmounts
	{
		int* amounts = new int[ECapitalType::ECapitalTypeCount];

		int operator[](size_t idx)
		{
			return amounts[idx];
		}

		inline const int Size() { return ECapitalType::ECapitalTypeCount; }

		// Assign
		inline CapitalAmounts& operator=(const CapitalAmounts& rhs)
		{
			for (size_t i = 0; i < ECapitalType::ECapitalTypeCount; i++)
			{
				amounts[i] = rhs.amounts[i];
			}
		}

		inline CapitalAmounts& operator+=(const CapitalAmounts& rhs)
		{
			for (size_t i = 0; i < ECapitalType::ECapitalTypeCount; i++)
			{
				amounts[i] += rhs.amounts[i];
			}
		}

		inline CapitalAmounts& operator-=(const CapitalAmounts& rhs)
		{
			for (size_t i = 0; i < ECapitalType::ECapitalTypeCount; i++)
			{
				amounts[i] -= rhs.amounts[i];
			}
		}

		// Comparison
		inline bool operator>=(const CapitalAmounts& rhs)
		{
			for (size_t i = 0; i < ECapitalType::ECapitalTypeCount; i++)
			{
				if (amounts[i] < rhs.amounts[i])
					return false;
			}

			return true;
		}
	};

	struct ActionCost
	{
		float time;
		CapitalAmounts capital;
	};
}

