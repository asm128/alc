#include "alc_static_bus.h"
#include "alc_raster_manager.h"
#include "llc_system_event.h"
#include "llc_color.h"
#include "llc_event_raster.h"

#include "llc_geometry2.h"
//#include "llc_array_static.h"
//#include "llc_astatic_n2.h"

#include <Wire.h>

extern "C" {
#include "utility/twi.h"
}

#include "MCUFRIEND_kbv.h"

LLC_USING_TYPEINT();

template<uint16_t _nInput, uint16_t _nOutput>
static	::llc::error_t	readWire		(TwoWire & wire, ::alc::SStaticBusState<_nInput, _nOutput> & spi, int byteCount) {
	return spi.ReadWire(byteCount
		, [&wire]() { return wire.available(); }
		, [&wire]() { return (char)wire.read(); }
		);
}

//template<uint16_t _nInput, uint16_t _nOutput>
//::llc::error_t	readWire				(::llc::SStaticBusState<_nInput, _nOutput> & spi, int byteCount) {
//	uint16_t		iByte	= spi.InputBufferEnd = 0;
//	for(cnst uint16_t stop = ::llc::clamped((uint16_t)byteCount, (uint16_t)0, (uint16_t)::llc::size(spi.InputBuffer)); spi.InputBufferEnd < stop; ++iByte, ++spi.InputBufferEnd) {
//		ree_if(false == Wire.available(), "Expected bytes: %i, Read: %u.", byteCount, iByte);
//		spi.InputBuffer[spi.InputBufferEnd] = Wire.read();
//	}
//	return iByte;
//}
static	::llc::error_t	loadEvents		(::alc::SRasterManager & raster, ::llc::vcu0_t bytes, bool isOnline) {
	::llc::error_t			result			= -1;
	while(bytes.size() && raster.RasterEventCount < ::alc::MAX_RASTER_EVENTS) {
		::llc::SEView<::llc::SYSTEM_EVENT>	sysEvent;
		bes_if_failed(sysEvent.Load(bytes));

		switch(sysEvent.Type) {
		case ::llc::SYSTEM_EVENT_Raster:
			bes_if_failed(result = raster.RasterEvents[raster.RasterEventCount].Load(sysEvent.Data));
			raster.Busy	= ++raster.RasterEventCount;
			break;  
		case ::llc::SYSTEM_EVENT_Device:
			{
				cnst bool	inputTooSmall = sysEvent.Data.size() < sizeof(::llc::COMMAND);
				bes_if_failed(result = (inputTooSmall ? -1 : 0));
				::llc::COMMAND	deviceStatus = *(cnst ::llc::COMMAND*)sysEvent.Data.begin();
				info_printf("Device status: %li(0x%lX).", deviceStatus, deviceStatus);
				bes_if_failed(result = deviceStatus);
				switch(deviceStatus) {
				case ::llc::COMMAND_Wake_up:
					result	= ::llc::COMMAND_Connected;
					info_printf("result: %li(0x%lX).", deviceStatus, deviceStatus);
					break;
				}
			}
			break;
		}
		if_failed(result) 
			break;
	}
	raster.Busy	= raster.RasterEventCount;
	//gpk_necall(result, "0x%x-%u", result, result);
	return (result == ::llc::COMMAND_Connected) ? 1 : 0;
}

#pragma pack(push, 1)

struct STFTDisplay : public MCUFRIEND_kbv {// public Adafruit_GFX {
	//inline STFTDisplay(uint16_t width = 480, uint16_t height = 320) : Adafruit_GFX(height, width) { } // we can not access GPIO pins until AHB has been enabled.
	using MCUFRIEND_kbv::fillTriangle;
	using MCUFRIEND_kbv::drawTriangle;
	using MCUFRIEND_kbv::fillCircle;
	using MCUFRIEND_kbv::drawCircle;
	using MCUFRIEND_kbv::fillRect;
	using MCUFRIEND_kbv::drawRect;
	using MCUFRIEND_kbv::drawXBitmap;
	using MCUFRIEND_kbv::drawBitmap;

	void	fillCircle				(cnst ::llc::circleu1_t & a, uint16_t color)												{ MCUFRIEND_kbv::fillCircle(a.Center.x, a.Center.y, a.Radius, color); }
	void	drawCircle				(cnst ::llc::circleu1_t & a, uint16_t color)												{ MCUFRIEND_kbv::drawCircle(a.Center.x, a.Center.y, a.Radius, color); }
	void	fillRectangle			(cnst ::llc::rect2u1_t & a, uint16_t color)													{ MCUFRIEND_kbv::fillRect(a.Offset.x, a.Offset.y, a.Size.x, a.Size.y, color); }
	void	drawRectangle			(cnst ::llc::rect2u1_t & a, uint16_t color)													{ MCUFRIEND_kbv::drawRect(a.Offset.x, a.Offset.y, a.Size.x, a.Size.y, color); }
	void	fillRectangle			(cnst ::llc::n2u1_t & offset, cnst ::llc::n2u1_t & size, uint16_t color)					{ MCUFRIEND_kbv::fillRect(offset.x, offset.y, size.x, size.y, color); }
	void	drawRectangle			(cnst ::llc::n2u1_t & offset, cnst ::llc::n2u1_t & size, uint16_t color)					{ MCUFRIEND_kbv::drawRect(offset.x, offset.y, size.x, size.y, color); }
	void	fillRectangle			(cnst ::llc::minmax2u1_t & minMax, uint16_t color) 											{ MCUFRIEND_kbv::fillRect(minMax.Min.x, minMax.Min.y, 1 + minMax.Max.x - minMax.Min.x, 1 + minMax.Max.y - minMax.Min.y, color); }
	void	drawRectangle			(cnst ::llc::minmax2u1_t & minMax, uint16_t color) 											{ MCUFRIEND_kbv::drawRect(minMax.Min.x, minMax.Min.y, 1 + minMax.Max.x - minMax.Min.x, 1 + minMax.Max.y - minMax.Min.y, color); }
	void	fillTriangle			(cnst ::llc::n2u1_t & a, cnst ::llc::n2u1_t & b, cnst ::llc::n2u1_t & c, uint16_t color)	{ MCUFRIEND_kbv::fillTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color); }
	void	drawTriangle			(cnst ::llc::n2u1_t & a, cnst ::llc::n2u1_t & b, cnst ::llc::n2u1_t & c, uint16_t color)	{ MCUFRIEND_kbv::drawTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color); }
	void	fillTriangle			(cnst ::llc::tri2u1_t & triangle, uint16_t color)											{ fillTriangle(triangle.A, triangle.B, triangle.C, color); }
	void	drawTriangle			(cnst ::llc::tri2u1_t & triangle, uint16_t color)											{ drawTriangle(triangle.A, triangle.B, triangle.C, color); }

	void clearScreen(::llc::bgra color) {
		fillRect(0, 24, width(), height(), color);
		setCursor(5, 30);
	}

	void drawXBitmap(int16_t x, int16_t y, cnst uint16_t bitmap[], int16_t w, int16_t h) {
		startWrite();
		for (int16_t j = 0; j < h; j++, y++) {
			for (int16_t i = 0; i < w; i++) {
				uint16_t color = pgm_read_word(&bitmap[j * w + i]);
			// Nearly identical to drawBitmap(), only the bit order is reversed here (left-to-right = LSB to MSB):
				writePixel(x + i, y, color);
			}
		}
		endWrite();
	}
};



stxp	uint8_t	INPUT_BUFFER_SIZE	= 32;
stxp	uint8_t	OUTPUT_BUFFER_SIZE	= 32;

static
struct SRenderApp : alc::SStaticBusState<INPUT_BUFFER_SIZE, OUTPUT_BUFFER_SIZE> {
	STFTDisplay			Display				= {};
	alc::SRasterManager	RasterManager		= {};

	::llc::n2u2_t		TouchPointer[4]		= {};

	//uint64_t			TouchPointer0X		: 10;
	//uint64_t			TouchPointer1X		: 10;
	//uint64_t			TouchPointer2X		: 10;
	//uint64_t			TouchPointer3X		: 10;
	//uint64_t			TouchPointer0Y		: 10;
	//uint64_t			TouchPointer1Y		: 10;
	//uint64_t			TouchPointer2YHi	: 4;
	//uint16_t			TouchPointer2YLo	: 6;
	//uint16_t			TouchPointer3Y		: 10;
} g_app;
#pragma pack(pop)


static	::llc::error_t	drawHeader	() {
	g_app.Display.fillRect(1, 1, 477, 22, TFT_RED);
	g_app.Display.drawRect(0, 0, 479, 24, TFT_YELLOW);
	g_app.Display.setCursor(5, 5);
	//g_app.Display.setTextColor(TFT_WHITE);
	g_app.Display.setTextSize(2);
	g_app.Display.println(F("I2C Drawing Demo"));
	g_app.Display.setCursor(5, 30);
	return 0;
}

static	::llc::error_t	spiTransmit		(cnst ::llc::vcu0_t & bytesToSend)				{
	llc_necall(-1 * twi_transmit(bytesToSend.begin(), bytesToSend.size()), "Failed to send %u bytes.", bytesToSend.size());
	return 0;
}

static	void	requestEvent	()				{
	::llc::SDeviceResult eventToSend;
	if(g_app.Online) {
		eventToSend.DeviceEvent	= (g_app.RasterManager.Busy || g_app.Receiving || g_app.RasterManager.RasterEventCount) ? ::llc::RESULT_Busy : ::llc::RESULT_Ok;
		if(eventToSend.DeviceEvent)
			info_printf("%s|%s|%u -> %i", ::llc::bool2char(g_app.RasterManager.Busy), ::llc::bool2char(g_app.Receiving), g_app.RasterManager.RasterEventCount, eventToSend.DeviceEvent);
	}
	else {
		eventToSend.DeviceEvent	= ::llc::RESULT_Restart;
		info_printf("Sending restart event.");
	}
	::spiTransmit({(cnst uint8_t*)&eventToSend, sizeof(::llc::SDeviceResult)});
}

static	void	receiveEvent	(int byteCount) {
	//ef_if(g_app.RasterManager.Busy || g_app.RasterManager.RasterEventCount, "%s || %u", ::llc::bool2char(g_app.RasterManager.Busy), g_app.RasterManager.RasterEventCount);

	g_app.Receiving	= true;
	delayMicroseconds(1);

	es_if_failed(::readWire(Wire, g_app, byteCount));
	if(g_app.InputBufferEnd) {
		//info_printf("%u bytes received.", g_app.InputBufferEnd);
		::llc::vcu0_t		bytes	= g_app.InputBuffer;
		e_if(llc::failed(bytes.slice(bytes, 0, g_app.InputBufferEnd)))
		else {
			::llc::error_t		result; 
			e_if(llc::failed(result = ::loadEvents(g_app.RasterManager, bytes, g_app.Online)))
			else {
				switch(result) {
				case 1:
				case ::llc::COMMAND_Connected:
					g_app.Online = true;//!g_app.Online;
					info_printf("Device is now online.");
					break;
				}
			}
		}
		info_printf("Loaded %u events.", g_app.RasterManager.RasterEventCount);
	}

	g_app.Receiving	= false;
}

//static	::llc::error_t	setupI2CAsMaster() { Wire.begin(); return 0; }
static	::llc::error_t	setupI2CAsSlave	() {
	Wire.begin(9);
	// Attach a function to trigger when something is received.
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
	return 0;
}


extern ::llc::log_write_t	llc_log_write	;
extern ::llc::log_print_t	llc_log_print	;
extern ::llc::log_print_P_t	llc_log_print_P	;

static	HardwareSerial	& GPKLogStream	= Serial;
static	::llc::error_t	serial_write	(sc_c* text, u2_t textLen)	{ return GPKLogStream.write(text, textLen); }
static	::llc::error_t	serial_print	(sc_c* text)						{ return GPKLogStream.print(text); }
static	::llc::error_t	serial_print_P	(cnst __FlashStringHelper * text)		{ return GPKLogStream.print(text); }

void setup() {
	GPKLogStream.begin(9600);
//
	//gpk_debug_printf	= serial_write;
	llc_log_print_P		= serial_print_P;
	llc_log_print		= serial_print;
	llc_log_write		= serial_write;

	cnst uint16_t ID = g_app.Display.readID();
	info_printf("TFT Id: %u - 0x%x.", ID, ID);
	g_app.Display.begin(ID);
	g_app.Display.setRotation(1);
	g_app.Display.fillScreen(TFT_BLACK);
	::drawHeader();
	es_if_failed(::setupI2CAsSlave());
}

void			loop	() {
	res_if(g_app.Receiving);
	if(false == g_app.RasterManager.Busy && 0 == g_app.RasterManager.RasterEventCount) {
		::llc::sleep(1);
		//ri_if("Busy" || "RasterEventCount", "(%s || %u)", ::llc::bool2char(g_app.RasterManager.Busy), g_app.RasterManager.RasterEventCount);
		return;
	}
	//info_printf("Busy(%s) || (%u)", ::llc::bool2char(g_app.RasterManager.Busy), g_app.RasterManager.RasterEventCount);

	for(u2_t iEvent = 0, eventCount = g_app.RasterManager.RasterEventCount; iEvent < eventCount; ++iEvent) {
		es_if_failed(rasterEventProcess(g_app.Display, g_app.RasterManager, g_app.RasterManager.RasterEvents[iEvent]));
	}
	g_app.RasterManager.Busy = (g_app.RasterManager.RasterEventCount = 0);
	::llc::sleep(1);
}

// #include "alc_static_bus.h"
// #include "alc_raster_manager.h"
// #include "gpk_system_event.h"
// #include "gpk_color.h"
// #include "gpk_event_raster.h"

// #include "gpk_geometry2.h"
// //#include "gpk_array_static.h"
// //#include "gpk_astatic_n2.h"

// #include <Wire.h>

// extern "C" {
// #include "utility/twi.h"
// }

// #include "MCUFRIEND_kbv.h"

// template<tpnm _tPOD, uint16_t _nInput, uint16_t _nOutput>
// static	::llc::error_t	readWire		(TwoWire & wire, ::llc::SStaticBusState<_tPOD, _nInput, _nOutput> & spi, int byteCount) {
// 	return spi.Input.read(byteCount
// 		, [&wire]() { return wire.available(); }
// 		, [&wire]() { return (_tPOD)wire.read(); }
// 		);
// }

// static	::llc::error_t	loadEvents		(::llc::SRasterManager & raster, llc::vcu0_c bytes, bool isOnline) {
// 	::llc::error_t			result			= -1;
// 	while(bytes.size() && raster.RasterEventCount < ::llc::MAX_RASTER_EVENTS) {
// 		::llc::SEView<::llc::SYSTEM_EVENT>	sysEvent;
// 		bes_if_failed(sysEvent.Load(bytes));

// 		switch(sysEvent.Type) {
// 		case ::llc::SYSTEM_EVENT_Raster:
// 			bes_if_failed(result = raster.RasterEvents[raster.RasterEventCount].Load(sysEvent.Data));
// 			raster.Busy	= ++raster.RasterEventCount;
// 			break;  
// 		case ::llc::SYSTEM_EVENT_Device:
// 			{
// 				cnst bool	inputTooSmall = sysEvent.Data.size() < sizeof(::llc::COMMAND);
// 				bes_if_failed(result = (inputTooSmall ? -1 : 0));
// 				::llc::COMMAND	deviceStatus = *(cnst ::llc::COMMAND*)sysEvent.Data.begin();
// 				info_printf("Device status: %li(0x%lX).", deviceStatus, deviceStatus);
// 				bes_if_failed(result = deviceStatus);
// 				switch(deviceStatus) {
// 				case ::llc::COMMAND_Wake_up:
// 					result	= ::llc::COMMAND_Connected;
// 					info_printf("result: %li(0x%lX).", deviceStatus, deviceStatus);
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 		if_failed(result) 
// 			break;
// 	}
// 	raster.Busy	= raster.RasterEventCount;
// 	//gpk_necall(result, "0x%x-%u", result, result);
// 	return (result == ::llc::COMMAND_Connected) ? 1 : 0;
// }

// #pragma pack(push, 1)

// struct STFTDisplay : public MCUFRIEND_kbv {// public Adafruit_GFX {
// 	//inline STFTDisplay(uint16_t width = 480, uint16_t height = 320) : Adafruit_GFX(height, width) { } // we can not access GPIO pins until AHB has been enabled.
// 	using MCUFRIEND_kbv::fillTriangle;
// 	using MCUFRIEND_kbv::drawTriangle;
// 	using MCUFRIEND_kbv::fillCircle;
// 	using MCUFRIEND_kbv::drawCircle;
// 	using MCUFRIEND_kbv::fillRect;
// 	using MCUFRIEND_kbv::drawRect;
// 	using MCUFRIEND_kbv::drawXBitmap;
// 	using MCUFRIEND_kbv::drawBitmap;

// 	void fillCircle(cnst ::llc::circleu1_t & a, uint16_t color)													{ MCUFRIEND_kbv::fillCircle(a.Center.x, a.Center.y, a.Radius, color); }
// 	void drawCircle(cnst ::llc::circleu1_t & a, uint16_t color)													{ MCUFRIEND_kbv::drawCircle(a.Center.x, a.Center.y, a.Radius, color); }
// 	void fillRectangle(cnst ::llc::rect2u1_t & a, uint16_t color)												{ MCUFRIEND_kbv::fillRect(a.Offset.x, a.Offset.y, a.Size.x, a.Size.y, color); }
// 	void drawRectangle(cnst ::llc::rect2u1_t & a, uint16_t color)												{ MCUFRIEND_kbv::drawRect(a.Offset.x, a.Offset.y, a.Size.x, a.Size.y, color); }
// 	void fillRectangle(cnst ::llc::n2u1_t & offset, cnst ::llc::n2u1_t & size, uint16_t color)					{ MCUFRIEND_kbv::fillRect(offset.x, offset.y, size.x, size.y, color); }
// 	void drawRectangle(cnst ::llc::n2u1_t & offset, cnst ::llc::n2u1_t & size, uint16_t color)					{ MCUFRIEND_kbv::drawRect(offset.x, offset.y, size.x, size.y, color); }
// 	void fillRectangle(cnst ::llc::minmax2u1_t & minMax, uint16_t color) 										{ MCUFRIEND_kbv::fillRect(minMax.Min.x, minMax.Min.y, 1 + minMax.Max.x - minMax.Min.x, 1 + minMax.Max.y - minMax.Min.y, color); }
// 	void drawRectangle(cnst ::llc::minmax2u1_t & minMax, uint16_t color) 										{ MCUFRIEND_kbv::drawRect(minMax.Min.x, minMax.Min.y, 1 + minMax.Max.x - minMax.Min.x, 1 + minMax.Max.y - minMax.Min.y, color); }
// 	void fillTriangle(cnst ::llc::n2u1_t & a, cnst ::llc::n2u1_t & b, cnst ::llc::n2u1_t & c, uint16_t color)	{ MCUFRIEND_kbv::fillTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color); }
// 	void drawTriangle(cnst ::llc::n2u1_t & a, cnst ::llc::n2u1_t & b, cnst ::llc::n2u1_t & c, uint16_t color)	{ MCUFRIEND_kbv::drawTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color); }
// 	void fillTriangle(cnst ::llc::tri2u1_t & triangle, uint16_t color)											{ fillTriangle(triangle.A, triangle.B, triangle.C, color); }
// 	void drawTriangle(cnst ::llc::tri2u1_t & triangle, uint16_t color)											{ drawTriangle(triangle.A, triangle.B, triangle.C, color); }

// 	void clearScreen(::llc::bgra color) {
// 		fillRect(0, 24, width(), height(), color);
// 		setCursor(5, 30);
// 	}

// 	void drawXBitmap(int16_t x, int16_t y, cnst uint16_t bitmap[], int16_t w, int16_t h) {
// 		startWrite();
// 		for (int16_t j = 0; j < h; j++, y++) {
// 			for (int16_t i = 0; i < w; i++) {
// 				uint16_t color = pgm_read_word(&bitmap[j * w + i]);
// 			// Nearly identical to drawBitmap(), only the bit order is reversed here (left-to-right = LSB to MSB):
// 				writePixel(x + i, y, color);
// 			}
// 		}
// 		endWrite();
// 	}
// };



// stxp	uint8_t	INPUT_BUFFER_SIZE	= 32;
// stxp	uint8_t	OUTPUT_BUFFER_SIZE	= 32;

// static
// struct SRenderApp : alc::SStaticBusState<uint8_t, INPUT_BUFFER_SIZE, OUTPUT_BUFFER_SIZE> {
// 	STFTDisplay			Display				= {};
// 	alc::SRasterManager	RasterManager		= {};

// 	::llc::n2u32		TouchPointer[4]		= {};

// 	//uint64_t			TouchPointer0X		: 10;
// 	//uint64_t			TouchPointer1X		: 10;
// 	//uint64_t			TouchPointer2X		: 10;
// 	//uint64_t			TouchPointer3X		: 10;
// 	//uint64_t			TouchPointer0Y		: 10;
// 	//uint64_t			TouchPointer1Y		: 10;
// 	//uint64_t			TouchPointer2YHi	: 4;
// 	//uint16_t			TouchPointer2YLo	: 6;
// 	//uint16_t			TouchPointer3Y		: 10;
// } g_app;
// #pragma pack(pop)


// static	::llc::error_t	drawHeader	() {
// 	g_app.Display.fillRect(1, 1, 477, 22, TFT_RED);
// 	g_app.Display.drawRect(0, 0, 479, 24, TFT_YELLOW);
// 	g_app.Display.setCursor(5, 5);
// 	//g_app.Display.setTextColor(TFT_WHITE);
// 	g_app.Display.setTextSize(2);
// 	g_app.Display.println(F("I2C Drawing Demo"));
// 	g_app.Display.setCursor(5, 30);
// }

// static	::llc::error_t	spiTransmit		(cnst llc::vcu0_c & bytesToSend)				{
// 	gpk_necall(-1 * twi_transmit(bytesToSend.begin(), bytesToSend.size()), "Failed to send %u bytes.", bytesToSend.size());
// 	return 0;
// }

// static	void	requestEvent	()				{
// 	::llc::SDeviceResult eventToSend;
// 	if(g_app.Online) {
// 		eventToSend.DeviceEvent	= (g_app.RasterManager.Busy || g_app.Receiving || g_app.RasterManager.RasterEventCount) ? ::llc::RESULT_Busy : ::llc::RESULT_Ok;
// 		if(eventToSend.DeviceEvent)
// 			info_printf("%s|%s|%u -> %i", ::llc::bool2char(g_app.RasterManager.Busy), ::llc::bool2char(g_app.Receiving), g_app.RasterManager.RasterEventCount, eventToSend.DeviceEvent);
// 	}
// 	else {
// 		eventToSend.DeviceEvent	= ::llc::RESULT_Restart;
// 		info_printf("Sending restart event.");
// 	}
// 	::spiTransmit({(cnst uint8_t*)&eventToSend, sizeof(::llc::SDeviceResult)});
// }

// static	void	receiveEvent	(int byteCount) {
// 	//ef_if(g_app.RasterManager.Busy || g_app.RasterManager.RasterEventCount, "%s || %u", ::llc::bool2char(g_app.RasterManager.Busy), g_app.RasterManager.RasterEventCount);

// 	g_app.Receiving	= true;
// 	delayMicroseconds(1);

// 	es_if_failed(::readWire(Wire, g_app, byteCount));
// 	if(g_app.Input.used()) {
// 		//info_printf("%u bytes received.", g_app.Input.used());
// 		::llc::astu8<33> 	received	= {};
// 		llc::vcu0_c			left		= {};
// 		llc::vcu0_c			right		= {};
// 		g_app.Input.pair(left, right);
// 		memcpy(received.begin(), left.begin(), left.size());
// 		if(right.size())
// 			memcpy(&received[left.size()], right.begin(), right.size());
		
// 		llc::vcu0_c			bytes		= received;
// 		e_if(llc::failed(received.slice(bytes, 0, g_app.Input.used())))
// 		else {
// 			::llc::error_t		result; 
// 			e_if(llc::failed(result = ::loadEvents(g_app.RasterManager, bytes, g_app.Online)))
// 			else {
// 				switch(result) {
// 				case 1:
// 				case ::llc::COMMAND_Connected:
// 					g_app.Online = true;//!g_app.Online;
// 					info_printf("Device is now online.");
// 					break;
// 				}
// 			}
// 		}
// 		info_printf("Loaded %u events.", g_app.RasterManager.RasterEventCount);
// 	}

// 	g_app.Receiving	= false;
// }

// //static	::llc::error_t	setupI2CAsMaster() { Wire.begin(); return 0; }
// static	::llc::error_t	setupI2CAsSlave	() {
// 	Wire.begin(9);
// 	// Attach a function to trigger when something is received.
// 	Wire.onReceive(receiveEvent);
// 	Wire.onRequest(requestEvent);
// 	return 0;
// }


// extern ::llc::log_write_t	gpk_log_write	;
// extern ::llc::log_print_t	gpk_log_print	;
// extern ::llc::log_print_P_t	gpk_log_print_P	;

// static HardwareSerial	& GPKLogStream	= Serial;
// static	void	serial_write	(sc_c* text, u2_t textLen)	{ GPKLogStream.write(text, textLen); }
// static	void	serial_print	(sc_c* text)						{ GPKLogStream.print(text); }
// static	void	serial_print_P	(cnst __FlashStringHelper * text)		{ GPKLogStream.print(text); }

// void setup() {
// 	GPKLogStream.begin(9600);
// //
// 	//gpk_debug_printf	= serial_write;
// 	gpk_log_print_P		= serial_print_P;
// 	gpk_log_print		= serial_print;
// 	gpk_log_write		= serial_write;

// 	cnst uint16_t ID = g_app.Display.readID();
// 	info_printf("TFT Id: %u - 0x%x.", ID, ID);
// 	g_app.Display.begin(ID);
// 	g_app.Display.setRotation(1);
// 	g_app.Display.fillScreen(TFT_BLACK);
// 	::drawHeader();
// 	es_if_failed(::setupI2CAsSlave());
// }

// void			loop	() {
// 	res_if(g_app.Receiving);
// 	if(false == g_app.RasterManager.Busy && 0 == g_app.RasterManager.RasterEventCount) {
// 		::llc::sleep(1);
// 		//ri_if("Busy" || "RasterEventCount", "(%s || %u)", ::llc::bool2char(g_app.RasterManager.Busy), g_app.RasterManager.RasterEventCount);
// 		return;
// 	}
// 	//info_printf("Busy(%s) || (%u)", ::llc::bool2char(g_app.RasterManager.Busy), g_app.RasterManager.RasterEventCount);

// 	for(u2_t iEvent = 0, eventCount = g_app.RasterManager.RasterEventCount; iEvent < eventCount; ++iEvent) {
// 		es_if_failed(rasterEventProcess(g_app.Display, g_app.RasterManager, g_app.RasterManager.RasterEvents[iEvent]));
// 	}
// 	g_app.RasterManager.Busy = g_app.RasterManager.RasterEventCount = 0;
// 	::llc::sleep(1);
// }
