#pragma once
#include <vector>

class FloatingAverager
{
public:
	/**
	 * \brief Generates the floating averager.
	 * \param amountOfElements Amount of elements to create the floating value from.
	 */
	FloatingAverager(int amountOfElements);

	/**
	 * \brief Inserts a new element into the averager and returns the averaged value.
	 * \param value New value to insert.
	 * \return Result of floating average
	 */
	float NewElement(float value);

private:
	std::vector<float> m_ringCache;
	int m_amountOfElements;
	int m_writingPosition;
	float m_sum;
};

