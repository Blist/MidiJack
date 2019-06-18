#include <string>
#include <queue>
#include <mutex>
#include <vector>
#include <iostream>
#include <cstdlib>
#include "RtMidi.h"

#if defined _WIN32 || defined __CYGWIN__
	#define EXPORT_API extern "C" __declspec(dllexport)
#else
	#define EXPORT_API extern "C" 
#endif

namespace
{
	// MIDI message storage class
	class MidiMessage
	{
		unsigned int source_;
		uint8_t status_;
		uint8_t data_[2];

	public:

		MidiMessage(unsigned int source, uint8_t status)
			: source_(source), status_(status)
		{
			data_[0] = data_[1] = 0;
		}

		void SetData(int offs, uint8_t byte)
		{
			if (offs < 2) data_[offs] = byte;
		}

		uint64_t Encode64Bit() const
		{
			uint64_t ul = (uint32_t)source_;
			ul |= (uint64_t)status_ << 32;
			ul |= (uint64_t)data_[0] << 40;
			ul |= (uint64_t)data_[1] << 48;
			return ul;
		}
	};

	uint32_t used_midijack = 0;
	std::vector<RtMidiIn> midiIns;
	std::vector<std::string> names;

	// Incoming MIDI message queue
	std::queue<MidiMessage> message_queue;
	std::mutex message_queue_lock;

	// Reset-is-required flag
	bool reset_required = true;
}

namespace
{
	// Reset the status if required.
	// Returns false when something goes wrong.

	void MIDIReadProc(double timeStamp, std::vector<unsigned char> *message, void* userData)
	{        
		message_queue_lock.lock();

		for (uint32_t offs = 0; offs < message->size();) {
			MidiMessage midiMessage(*((uint32_t *)userData), message->at(offs++));
			for (uint32_t dc = 0; offs < message->size() && (message->at(offs) < 0x80); dc++, offs++)
				midiMessage.SetData(dc, message->at(offs));
			message_queue.push(midiMessage);
		}

		message_queue_lock.unlock();
	}


	bool ResetIfRequired()
	{
		if (!reset_required) return true;

		RtMidiIn *midiIn = new RtMidiIn();

		// Enumerate the all MIDI sources.
		uint32_t sourceCount = midiIn->getPortCount();
		uint32_t i = 0;
		if (sourceCount == 0) return false;
		midiIns.resize(midiIn->getPortCount());
		midiIn->openPort(i);
		midiIn->setCallback(&MIDIReadProc, &i);
		midiIn->ignoreTypes( false, false, false );
		names.push_back(midiIn->getPortName(i));
		midiIns.push_back(*midiIn);
		i++;
		for (i=1; i < sourceCount; i++) {
			midiIn = new RtMidiIn();
			midiIn->openPort(i);
			midiIn->setCallback(&MIDIReadProc, &i);
			midiIn->ignoreTypes( false, false, false );
			names.push_back(midiIn->getPortName(i));
			midiIns.push_back(*midiIn);
		}
		reset_required = false;
		return true;
	}

	// Retrieve the name of a given source.
	std::string GetSourceName(uint32_t source_id)
	{
		if(names.size() >= source_id+1)
			return names.at(source_id);
		else 
			return "unknown";
	}
}

// Counts the number of endpoints.
extern "C" uint32_t MidiJackCountEndpoints()
{
	return static_cast<uint32_t>(names.size());
}

// Get the unique ID of an endpoint.
extern "C" uint32_t MidiJackGetEndpointIDAtIndex(uint32_t index)
{
	if (index < 0 || index >= (uint32_t)names.size()) return 0;
	return static_cast<uint32_t>(index);
}

// Get the name of an endpoint.
extern "C" const char* MidiJackGetEndpointName(uint32_t id)
{
	static std::string temp;
	temp = GetSourceName(id);
	return temp.c_str();
}

// Get the name of an endpoint.
extern "C" const void MidiJackToUse(uint32_t id)
{
	used_midijack = id;
	reset_required = true;
}

// Retrieve and erase an MIDI message data from the message queue.
extern "C" uint64_t MidiJackDequeueIncomingData()
{
	if (!ResetIfRequired() || message_queue.empty()) return 0;

	message_queue_lock.lock();
	auto m = message_queue.front();
	message_queue.pop();
	message_queue_lock.unlock();

	return m.Encode64Bit();
}
