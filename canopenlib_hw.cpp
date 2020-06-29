#include "windows.h"
#include "stdio.h"
#include "canopenlib_hw.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <conio.h>
#include <shlwapi.h>

#include <vector>
#include <memory>

#include "candle.h"

extern "C" {
#include "candle_ctrl_req.h"
}

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#ifdef _MANAGED
#pragma managed(pop)
#endif

struct py_candle_device;

struct py_candle_channel {
	py_candle_channel(py_candle_device& owner, uint8_t chanel = 0)
		: owner{ owner }, _ch{ chanel }, echo_on{false} {	}

	candle_capability_t capabilities() const {
		candle_capability_t res;

		candle_channel_get_capabilities(&owner, _ch, &res);

		return res;
	}

	uint8_t chanel() const { return _ch; }

	void setEchoEnabled(bool echoOn) {
		echo_on = echoOn;
	}

	bool isEcho() const { return echo_on; }

private:
	py_candle_device& owner;
	uint8_t _ch;
	bool echo_on;

	// RX FIFO
	//struct fifo_t* _fifo;
};

struct py_candle_device {
	py_candle_device(candle_handle dev)
		: _handle(dev), _channel{ std::make_unique<py_candle_channel>(*this) } {}

	candle_handle handle() const { return _handle; }
	py_candle_channel& chanel() const { return *_channel; }

	candle_capability_t capabilities() const { return _channel->capabilities(); }

private:
	// Candle device handle
	candle_handle _handle;

	// Open channels
	std::unique_ptr<py_candle_channel> _channel;

	// RX thread
	HANDLE _rx_thread;
	bool _rx_thread_stop_req;
};

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//------------------------------------------------------------------------

static std::vector<py_candle_device> py_candle_driver_list_devices()
{
	std::vector<py_candle_device> result;

	candle_list_handle clist;
	uint8_t num_devices;

	if (candle_list_scan(&clist)) {
		if (candle_list_length(clist, &num_devices)) {
			for (uint8_t i = 0; i < num_devices; ++i) {
				candle_handle dev;
				if (candle_dev_get(clist, i, &dev)) {
					result.emplace_back(dev);
				}
			}
		}
		candle_list_free(clist);
	}
	return result;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortLibraryInit(void)
{
	// prepare plugin to work
	return CANOPEN_OK;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortOpen(int port, canPortHandle* handle)
{
	// open device and write it's handle to "handle"
	auto devices = py_candle_driver_list_devices();
	if (port >= devices.size()) {
		return CANOPEN_OUT_OF_CAN_INTERFACES;
	}

	auto deviceOpening = new py_candle_device{ std::move(devices.at(port)) };

#if 0
	{
		auto capabilities = deviceOpening->capabilities();
		printf(R"V0G0N(feature = %d;
		 fclk_can = %d;
		 tseg1_min = %d;
		 tseg1_max = %d;
		 tseg2_min = %d;
		 tseg2_max = %d;
		 sjw_max = %d;
		 brp_min = %d;
		 brp_max = %d;
		 brp_inc = %d;\n)V0G0N", capabilities.feature, capabilities.fclk_can, capabilities.tseg1_min, capabilities.tseg1_max,
			capabilities.tseg2_min, capabilities.tseg2_max, capabilities.sjw_max, capabilities.brp_min,
			capabilities.brp_max, capabilities.brp_inc);
	}
#endif

	if (!candle_dev_open(deviceOpening->handle())) {
		return CANOPEN_ERROR_HW_NOT_CONNECTED;
	}

	*handle = reinterpret_cast<canPortHandle>(deviceOpening);

	return CANOPEN_OK;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortClose(canPortHandle handle)
{
	// close device with "handle" provided
	auto h = reinterpret_cast<py_candle_device*>(handle);

	auto res = candle_dev_close(h->handle()) ? CANOPEN_OK : CANOPEN_ERROR;

	candle_dev_free(h->handle());

	return res;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortBitrateSet(canPortHandle handle, int bitrate)
{
	// set device with "hande" new "bitrate"
	auto h = reinterpret_cast<py_candle_device*>(handle);

	return candle_channel_set_bitrate(h->handle(), h->chanel().chanel(), bitrate)
		? CANOPEN_OK
		: CANOPEN_UNSUPPORTED_BITRATE;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortEcho(canPortHandle handle, bool enabled)
{
	// set device with "hande" echo "enabled"
	auto h = reinterpret_cast<py_candle_device*>(handle);

	h->chanel().setEchoEnabled(enabled);

	return CANOPEN_OK;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortGoBusOn(canPortHandle handle)
{
	// set device with "hande" enabled
	auto h = reinterpret_cast<py_candle_device*>(handle);

	int mode = CANDLE_MODE_NORMAL;
	/*
	if (h->chanel().isEcho()) {
		mode |= CANDLE_MODE_LOOP_BACK;
	}*/

	return candle_channel_start(h->handle(),
		h->chanel().chanel(),
		mode
	) ? CANOPEN_OK : CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortGoBusOff(canPortHandle handle)
{
	// set device with "hande" disabled
	auto h = reinterpret_cast<py_candle_device*>(handle); 

	return candle_channel_stop(h->handle(),
		h->chanel().chanel()
	) ? CANOPEN_OK : CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortWrite(canPortHandle handle,
	long id,
	void* msg,
	unsigned int dlc,
	unsigned int flags)
{
	auto h = reinterpret_cast<py_candle_device*>(handle);
	candle_frame_t frame{
		0, id, dlc,  h->chanel().chanel(), flags, 0
	};
	std::memcpy(frame.data, msg, sizeof(frame.data));
	
	return candle_frame_send(h->handle(), h->chanel().chanel(), &frame)
		? CANOPEN_ERROR
		: CANOPEN_ERROR_DRIVER;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortRead(canPortHandle handle,
	long* id,
	void* msg,
	unsigned int* dlc,
	unsigned int* flags)
{
	candle_frame_t frame;
	auto h = reinterpret_cast<py_candle_device*>(handle);

	if (!candle_frame_read(h->handle(), &frame, 10)) {
		switch (candle_dev_last_error(h->handle()))
		{
		case CANDLE_ERR_READ_TIMEOUT:
			return CANOPEN_ERROR_NO_MESSAGE;
		case CANDLE_ERR_READ_WAIT:
			return CANOPEN_ASYNC_TRANSFER;
		case CANDLE_ERR_READ_RESULT:
		case CANDLE_ERR_READ_SIZE:
			return CANOPEN_INTERNAL_STATE_ERROR;
		default:
			return CANOPEN_ERROR_DRIVER;
		}
	}

	*id = frame.can_id;
	std::memcpy(msg, frame.data, sizeof(frame.data));
	*dlc = frame.can_dlc;
	*flags = frame.flags;

	return CANOPEN_OK;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus  __stdcall canPortGetSerialNumber(canPortHandle handle,
	char* buffer, int bufferLen)
{
	candle_device_config_t dconf;
	auto h = reinterpret_cast<py_candle_device*>(handle);

	if (!candle_ctrl_get_config(static_cast<candle_device_t*>(h->handle()), &dconf)) {
		return CANOPEN_ERROR;
	}
		
	snprintf(buffer, bufferLen, "%d", dconf.sw_version);

	return CANOPEN_OK;
}