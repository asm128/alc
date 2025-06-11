#include "alc_platform_globals.h"
#include "llc_event_raster.h"
#include "llc_view_color.h"
#include "llc_minmax_n2.h"
#include "llc_rect2.h"
#include "llc_tri2.h"
#include "llc_circle.h"

#ifndef ALC_RASTER_MANAGER_H
#define ALC_RASTER_MANAGER_H

namespace alc
{
#pragma pack(push, 1)
    stxp	n2u0_t	        PALETTE_METRICS		= {8, 8};
    stxp	uint16_t    	MAX_RASTER_EVENTS	= 8;

    //tplt<uint16_t _nMaxEvents = MAX_RASTER_EVENTS, uint8_t _sizePal = 255>
    struct SRasterManager {
        stxp	uint16_t	_nMaxEvents = MAX_RASTER_EVENTS;
        stxp	uint16_t	_sizePal	= PALETTE_METRICS.u1().Area();

        ::llc::SEViewRaster	RasterEvents[_nMaxEvents]	= {};
        vltl	uint8_t		RasterEventCount			= 0;

        vltl	bool		Busy						= {};

        n2u0_t		        Color						= {};
        n2u0_t		        Background					= {};
        uint16_t			Palette	[_sizePal]			=
                { 0, (uint16_t)::llc::bgra{::llc::BLUE}, (uint16_t)::llc::bgra{::llc::GREEN}, (uint16_t)::llc::bgra{::llc::CYAN}, (uint16_t)::llc::bgra{::llc::RED}, (uint16_t)::llc::bgra{::llc::MAGENTA}, (uint16_t)::llc::bgra{::llc::YELLOW}, (uint16_t)::llc::bgra{::llc::DARKGRAY}
                , (uint16_t)::llc::bgra{::llc::LIGHTGRAY * .5f}, (uint16_t)::llc::bgra{::llc::LIGHTBLUE}, (uint16_t)::llc::bgra{::llc::LIGHTGREEN}, (uint16_t)::llc::bgra{::llc::LIGHTCYAN}, (uint16_t)::llc::bgra{::llc::LIGHTRED}, (uint16_t)::llc::bgra{::llc::LIGHTMAGENTA}, (uint16_t)::llc::bgra{::llc::LIGHTYELLOW}, 0xFFFF
                , (uint16_t)::llc::bgra{::llc::DARKGRAY * .5f}, (uint16_t)::llc::bgra{::llc::DARKBLUE}, (uint16_t)::llc::bgra{::llc::DARKGREEN}, (uint16_t)::llc::bgra{::llc::DARKCYAN}, (uint16_t)::llc::bgra{::llc::DARKRED}, (uint16_t)::llc::bgra{::llc::DARKMAGENTA}, (uint16_t)::llc::bgra{::llc::DARKYELLOW}, (uint16_t)::llc::bgra{::llc::DARKGRAY}
                };
    };
#pragma pack(pop)

    template<typename _tDisplay>
    static	::llc::error_t	rasterOutline	(_tDisplay & display, ::llc::SEView<::llc::SHAPE_2D_TYPE> & shapeType, uint16_t color) {
        switch(shapeType.Type) {
        case ::llc::SHAPE_2D_TYPE_Rectangle: {
                const ::llc::rect2u1_t	& rect		= *(const ::llc::rect2u1_t*)shapeType.Data.begin();
                display.drawRectangle(rect, color);
            }
            break;
        case ::llc::SHAPE_2D_TYPE_Circle: {
                ::llc::circleu1_t		circle		= {};
                memcpy(&circle, shapeType.Data.begin(), ::llc::min((size_t)shapeType.Data.size(), (size_t)sizeof(circle)));
                display.drawCircle(circle, color);
            }
            break;
        case ::llc::SHAPE_2D_TYPE_Triangle: {
                const ::llc::tri2u1_t	& tri2	= *(const ::llc::tri2u1_t*)shapeType.Data.begin();
                display.drawTriangle(tri2, color);
            }
            break;
        }
        return 0;
    }

    template<typename _tDisplay>
    static	::llc::error_t	rasterFill		(_tDisplay & display, ::llc::SEView<::llc::SHAPE_2D_TYPE> & shapeType, uint16_t color) {
        switch(shapeType.Type) {
        case ::llc::SHAPE_2D_TYPE_Rectangle: {
                const ::llc::rect2u1_t	& rect	= *(const ::llc::rect2u1_t*)shapeType.Data.begin();
                display.fillRectangle(rect, color);
            }
            break;
        case ::llc::SHAPE_2D_TYPE_Circle: {
                ::llc::circleu1_t		circle		= {};
                memcpy(&circle, shapeType.Data.begin(), ::llc::min((size_t)shapeType.Data.size(), (size_t)sizeof(circle)));
                display.fillCircle(circle, color);
            }
            break;
        case ::llc::SHAPE_2D_TYPE_Triangle: {
                const ::llc::tri2u1_t	& tri2	= *(const ::llc::tri2u1_t*)shapeType.Data.begin();
                display.fillTriangle(tri2, color);
            }
            break;
        }
        return 0;
    }

    stin    llc::error_t  vc0u2color  (::llc::vcu0_c & input, ::llc::vcu1_c & palette, uint16_t & color) {
        switch(input.size()) {
        case 4: { ::llc::bgra colorToSet = {}; llc_necs(llc::vcu82bgra8(input, colorToSet)); color = (uint16_t)colorToSet; } break;
        case 3: { ::llc::bgr  colorToSet = {}; llc_necs(llc::vcu82bgr8 (input, colorToSet)); color = (uint16_t)colorToSet; } break;
        case 2: memcpy(&color, input.begin(), 2); break;
        case 1: 
            color = palette[input[0] % ::llc::size(palette)]; 
            break;
        }
        return 0;
    }
    tplt<tpnm _tDisplay>
    sttc	llc::error_t	rasterEventProcess	(_tDisplay & display, SRasterManager & raster, ::llc::SEViewRaster & eventRaster) {
        const uint16_t			color	= raster.Palette[raster.Color.Index(PALETTE_METRICS.x)];
        switch (eventRaster.Type) {
        case ::llc::EVENT_RASTER_Text	:
            if(eventRaster.Data.size()) {
                char buffer[64] = {};
                memcpy(buffer, eventRaster.Data.begin(), ::llc::min((uint8_t)63, (uint8_t)eventRaster.Data.size()));
                display.print(buffer);
            }
            break;
        case ::llc::EVENT_RASTER_Color	        : return vc0u2color(eventRaster.Data, raster.Palette, raster.Palette[raster.Color.Index(PALETTE_METRICS.x)]);
        case ::llc::EVENT_RASTER_FontColor	    : return vc0u2color(eventRaster.Data, raster.Palette, raster.Palette[raster.Color.Index(PALETTE_METRICS.x)]); 
        case ::llc::EVENT_RASTER_FontBackground	: return vc0u2color(eventRaster.Data, raster.Palette, raster.Palette[raster.Background.Index(PALETTE_METRICS.x)]);
        case ::llc::EVENT_RASTER_ColorIndex	: {
                const uint8_t	colorIndex	= eventRaster.Data.size() ? eventRaster.Data[0] : 0;
                raster.Color.x = colorIndex % PALETTE_METRICS.x;
                raster.Color.y = colorIndex / PALETTE_METRICS.x;
            }
            break;
        case ::llc::EVENT_RASTER_Palette	:
            memcpy(&raster.Palette[0], eventRaster.Data.begin(), ::llc::min(eventRaster.Data.size(), ::llc::size(raster.Palette)));
            break;
        case ::llc::EVENT_RASTER_Line	: {
                const ::llc::minmax2u1_t	& rect			= *(const ::llc::minmax2u1_t*)eventRaster.Data.begin();
                display.drawLine(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y, color);
            }
            break;
        case ::llc::EVENT_RASTER_Pixel	: {
                const ::llc::n2u1_t		& pixelCoord	= *(const ::llc::n2u1_t*)eventRaster.Data.begin();
                display.drawPixel(pixelCoord.x, pixelCoord.y, color);
            }
            break;
        case ::llc::EVENT_RASTER_Clear	: {
                const ::llc::bgra		& clearColor	= ((const ::llc::bgra*)eventRaster.Data.begin())[0];
                display.clearScreen(clearColor);
            }
            break;
        case ::llc::EVENT_RASTER_Outline	: {
                ::llc::SEView<::llc::SHAPE_2D_TYPE>	shapeType;
                llc_necs(shapeType.Load(eventRaster.Data));
                llc_necs(::alc::rasterOutline(display, shapeType, color));
            }
            break;
        case ::llc::EVENT_RASTER_Fill	: {
                ::llc::SEView<::llc::SHAPE_2D_TYPE>	shapeType;
                llc_necs(shapeType.Load(eventRaster.Data));
                llc_necs(::alc::rasterFill(display, shapeType, color));
            }
            break;
        }
        return 0;
    }

} // namespace 

#endif // ALC_RASTER_MANAGER_H
