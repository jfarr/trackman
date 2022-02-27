#include "PositionableMixingAudioSource.h"
#include "PositionableResamplingAudioSource.h"


PositionableMixingAudioSource::PositionableMixingAudioSource()
	: length(0)
	, looping(false)
{
}

PositionableMixingAudioSource::~PositionableMixingAudioSource()
{
	removeAllInputs();
}

void PositionableMixingAudioSource::addInputSource(PositionableAudioSource* input, bool deleteWhenRemoved, double sourceSampleRateToCorrectFor, int maxNumChannels)
{
	if (sourceSampleRateToCorrectFor > 0)
	{
		input = new PositionableResamplingAudioSource(input, deleteWhenRemoved, sourceSampleRateToCorrectFor, maxNumChannels);
		mixer.addInputSource(input, true);
	}
	else
	{
		mixer.addInputSource(input, deleteWhenRemoved);
	}
	inputs.add(input);
	updateLength();
}

void PositionableMixingAudioSource::removeInputSource(PositionableAudioSource* input)
{
	if (input != nullptr)
	{
		const int index = inputs.indexOf(input);
		inputs.remove(index);
		mixer.removeInputSource(input);
		updateLength();
	}
}

void PositionableMixingAudioSource::removeAllInputs()
{
	inputs.clear();
	mixer.removeAllInputs();
}

void PositionableMixingAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PositionableMixingAudioSource::releaseResources()
{
	mixer.releaseResources();
}

void PositionableMixingAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
	juce::int64 currentPos = getNextReadPosition();
	if (currentPos > length)
	{
		setNextReadPosition(currentPos);
	}
	mixer.getNextAudioBlock(info);
}

void PositionableMixingAudioSource::setNextReadPosition(juce::int64 newPosition)
{
	newPosition = looping ? newPosition % length : newPosition;
	for (int i = inputs.size(); --i >= 0;)
	{
		inputs.getUnchecked(i)->setNextReadPosition(newPosition);
	}
}

juce::int64 PositionableMixingAudioSource::getNextReadPosition() const
{
	juce::int64 nextPos = 0;
	for (int i = inputs.size(); --i >= 0;)
	{
		auto pos = inputs.getUnchecked(i)->getNextReadPosition();
		nextPos = juce::jmax(nextPos, pos);
	}
	return nextPos;
}

juce::int64 PositionableMixingAudioSource::getTotalLength() const
{
	return length;
}

void PositionableMixingAudioSource::updateLength()
{
	juce::int64 newLength = 0;
	for (int i = inputs.size(); --i >= 0;)
		if (inputs.getUnchecked(i)->getTotalLength() > newLength)
			newLength = inputs.getUnchecked(i)->getTotalLength();
	length = newLength;
}

bool PositionableMixingAudioSource::isLooping() const
{
	return looping;
}

void PositionableMixingAudioSource::setLooping(bool shouldLoop)
{
	looping = shouldLoop;
}
