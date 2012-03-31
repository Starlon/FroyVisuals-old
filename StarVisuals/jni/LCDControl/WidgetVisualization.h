/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of LCDControl.
 *
 * LCDControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCDControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCDControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WIDGET_VISUALIZATION_H__
#define __WIDGET_VISUALIZATION_H__

#include <json/json.h>
#include <xmmsclient/xmmsclient++.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>

#include "Property.h"
#include "Widget.h"
#include "RGBA.h"
#include "debug.h"

#include <libvisual/libvisual.h>

namespace LCD {

struct proxy {
    VisVideo *video;
    VisPalette *pal;
    const char *plugin;
    int16_t pcm_data[1024];
};

class LCDText;
class LCDGraphic;

class LCDCore;

class WidgetVisualization : public Widget {
    Q_OBJECT

    int direction_;
    int mono_;
    int style_;
    int update_;
    timeval last_update_;
    Xmms::Client client_;
    int vis_;
    std::vector<std::map<char, short> > history_;
    std::map<char, char> ch_;
    bool has_chars_;
    char *peak_buffer_;

/*
    QTimer *timer_;
    QTimer *morph_timer_;
*/
    // libvisual stuff
    VisSongInfo current_songinfo_;
    VisSongInfo final_songinfo_;
    proxy proxy_;
    void *buffer_;
    VisVideoDepth depth_;
    VisActor *actor_;
    VisInput *input_;
    VisAudio *audio_;
    std::string morph_;
    VisInput *input;
    int morph_timeout_;
    bool morph_chosen_;
    std::string skip_actors_;

    void (*Draw)(WidgetVisualization *);

    void DoParams();
    bool MyCurrentID( const int &id );
    bool MyGetPlaytime( const int &playtime );
    bool MyGetInfo( const Xmms::PropDict &propdict);
    bool ErrorHandler( const std::string &function, const std::string &error );

    public:
    WidgetVisualization(LCDCore *visitor, std::string name, 
        Json::Value *section, int row, int col, int layer);
    ~WidgetVisualization();
    void SetupChars();
    void Start();
    void Stop();
    int GetStyle() { return style_; }
    proxy GetProxy() { return proxy_; }
    void *GetBuffer() { return buffer_; }
    char *GetPeakBuffer() { return peak_buffer_; }
    bool HasChars() { return has_chars_; }
    VisVideoDepth GetDepth() { return depth_; }
    std::map<char, char> GetChars() { return ch_; }
    std::vector<std::map<char, short> > GetHistory() { return history_; }

    void Update();
    void UpdatePeak();
    void UpdatePCM();
    void UpdateSpectrum();
    void TextScroll() {}

    public slots:
    void VisualMorph();
    void Resize(int rows, int cols, int old_rows, int old_cols);
    void ResizeBefore(int rows, int cols);
    void ResizeAfter();

};

}; // End namespace

#define STYLE_PEAK 0
#define STYLE_PCM 1
#define STYLE_SPECTRUM 2

#endif
