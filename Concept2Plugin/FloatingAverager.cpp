#include "FloatingAverager.h"

FloatingAverager::FloatingAverager(int amountOfElements) :
	m_ringCache(amountOfElements), m_amountOfElements(amountOfElements),
	m_writingPosition(0), m_sum(0.0f)
{
}

float FloatingAverager::NewElement(float value)
{
	m_sum = m_sum + value - m_ringCache[m_writingPosition];
	m_ringCache[m_writingPosition] = value;
	m_writingPosition = (m_writingPosition + 1) % m_amountOfElements;
	return (m_sum / m_amountOfElements);
}




