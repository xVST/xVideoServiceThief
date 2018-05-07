/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2018 Xesc & Technology
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with xVideoServiceThief. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: Xesc <xeskuu.xvst@gmail.com>
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef __CONVERTVIDEOTYPES_H__
#define __CONVERTVIDEOTYPES_H__

/*! Output format conversion (DivX, MPEG1... ) */
enum OutputFormat
{
	ofAVI, ofWMV, ofMPEG1, ofMPEG2, ofMP4, ofMP4_hd, ofAppleiPod, ofSonyPSP, of3GP, ofMP3
};

/*! Video resolution */
enum VideoResolution
{
	vrOriginal, vr96x72, vr128x96, vr160x120, vr176x120, vr176x144,
	vr192x144, vr240x180, vr320x200, vr320x240, vr352x240, vr352x288, vr480x272,
	vr480x360, vr480x480, vr624x352, vr640x480, vr720x480, vr720x576
};

/*! Audio Sample Ratio */
enum AudioSampleRatio
{
	asrOriginal, asr22050, asr44100
};

/*! Video Frame Rate */
enum VideoFrameRate
{
	vfrOriginal, vfr15, vfr24, vfr25, vfr29_97, vfr30
};

/*! Output Quality */
enum OutputQuality
{
	oqLower_quality, oqLow_quality, oqNormal_quality,
	oqMedium_quality, oqGood_quality, oqSuperb_quality, oqSame_quality
};

struct VideoConversionConfig
{
	OutputFormat outputFormat;
	VideoResolution videoResolution;
	AudioSampleRatio audioSampleRatio;
	VideoFrameRate videoFrameRate;
	OutputQuality outputQuality;
};

#endif // __CONVERTVIDEOTYPES_H__

