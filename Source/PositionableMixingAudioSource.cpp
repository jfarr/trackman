#include "PositionableMixingAudioSource.h"


PositionableMixingAudioSource::PositionableMixingAudioSource()
	: position(0)
	, length(0)
	, looping(false)
{
}

PositionableMixingAudioSource::~PositionableMixingAudioSource()
{
	removeAllInputs();
}

void PositionableMixingAudioSource::addInputSource(PositionableAudioSource* input, bool deleteWhenRemoved)
{
	inputs.add(input);
	mixer.addInputSource(input, deleteWhenRemoved);
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
	mixer.getNextAudioBlock(info);
}

void PositionableMixingAudioSource::setNextReadPosition(juce::int64 newPosition)
{
	for (int i = inputs.size(); --i >= 0;)
		inputs.getUnchecked(i)->setNextReadPosition(newPosition);
	position = newPosition;
}

juce::int64 PositionableMixingAudioSource::getNextReadPosition() const
{
	return position;
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
	for (int i = inputs.size(); --i >= 0;)
		inputs.getUnchecked(i)->setLooping(shouldLoop);
}
