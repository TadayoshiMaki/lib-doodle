//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_range.h
//! \brief Range definitions
//---------------------------------------------------------------------------

#ifndef _mpj_range_h_
#define _mpj_range_h_

#include <algorithm>

#undef min
#undef max

namespace mpjsdk
{
	//! Range implementation. Both bounds are inclusive.
	template<typename V> class mpj_Range
	{
	private:
		//! Minimum value (first) and maximum value (second).
		std::pair<V, V> minMax;

	public:
		//! Initializes range.
		/*!
		\param a Either minimum value or maximum value.
		\param b Either minimum value or maximum value.
		It firstly check which one is bigger. So you don't have to check smaller or bigger.
		*/
		constexpr mpj_Range(const V& a, const V& b) : minMax(std::minmax(a, b))
		{
		}

		//! Get minimum and maximum values as \ref std::pair.
		constexpr const std::pair<V, V>& toPair() const { return minMax; }
		//! Minimum value
		constexpr V min() const { return minMax.first; }
		//! Maximum value
		constexpr V max() const { return minMax.second; }
		//! Determine whether value is in the range.
		constexpr bool isValueInRange(const V& v) const { return (min)() <= v && v <= (max)(); }
		//! Saturate value to fit into the range.
		constexpr V saturate(const V& v) const
		{
			if (v < (min)()) return (min)();
			if (v > (max)()) return (max)();
			return v;
		}
		constexpr bool operator==(const mpj_Range<V>& v) const { return minMax == v.minMax; }
		constexpr bool operator!=(const mpj_Range<V>& v) const { return minMax != v.minMax; }

		//! Determine whether the range is sub-range (or identical range) to another range.
		constexpr bool isSubrangeOfOrEqualTo(const mpj_Range<V>& v) const
		{
			return (v.min)() <= (min)() && (max)() <= (v.max)();
		}
		//! Saturate range to fit into the range.
		constexpr mpj_Range saturateRange(const mpj_Range<V>& v) const
		{
			return mpj_Range(saturate((min)()), saturate((max)()));
		}
	};
}

#endif // _mpj_range_h_
