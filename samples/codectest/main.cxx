/*
 * main.cxx
 *
 * OPAL application source file for testing codecs
 *
 * Main program entry point.
 *
 * Copyright (c) 2007 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#include "precompile.h"
#include "main.h"

#include <opal/patch.h>


PCREATE_PROCESS(CodecTest);

CodecTest::CodecTest()
  : PProcess("OPAL Audio/Video Codec Tester", "codectest", 1, 0, ReleaseCode, 0)
{
}


CodecTest::~CodecTest()
{
}


void CodecTest::Main()
{
  PArgList & args = GetArguments();

  args.Parse("b-bit-rate:"
             "c-crop."
             "C-rate-control."
             "D-display-device:"
             "-display-driver:"
             "F-audio-frames:"
             "G-grab-device:"
             "-grab-driver:"
             "-grab-format:"
             "-grab-channel:"
             "h-help."
             "m-suppress-marker."
             "M-force-marker."
             "O-option:"
             "p-payload-size:"
             "P-play-device:"
             "-play-driver:"
             "-play-buffers:"
             "r-frame-rate:"
             "R-record-device:"
             "-record-driver:"
             "s-frame-size:"
             "S-simultaneous:"
             "-count:"
             "-noprompt."
             "-snr."
#if PTRACING
             "o-output:"             "-no-output."
             "t-trace."              "-no-trace."
#endif
             , FALSE);

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('h') || args.GetCount() == 0) {
    PError << "usage: " << GetFile().GetTitle() << " [ options ] fmtname [ fmtname ]\n"
              "  where fmtname is the Media Format Name for the codec(s) to test, up to two\n"
              "  formats (one audio and one video) may be specified.\n"
              "\n"
              "Available options are:\n"
              "  --help                  : print this help message.\n"
              "  --record-driver drv     : audio recorder driver.\n"
              "  -R --record-device dev  : audio recorder device.\n"
              "  --play-driver drv       : audio player driver.\n"
              "  -P --play-device dev    : audio player device.\n"
              "  -F --audio-frames n     : audio frames per packet, default 1.\n"
              "  --play-buffers n        : audio player buffers, default 8.\n"
              "  --grab-driver drv       : video grabber driver.\n"
              "  -G --grab-device dev    : video grabber device.\n"
              "  --grab-format fmt       : video grabber format (\"pal\"/\"ntsc\")\n"
              "  --grab-channel num      : video grabber channel.\n"
              "  --display-driver drv    : video display driver to use.\n"
              "  -D --display-device dev : video display device to use.\n"
              "  -s --frame-size size    : video frame size (\"qcif\", \"cif\", WxH)\n"
              "  -r --frame-rate size    : video frame rate (frames/second)\n"
              "  -b --bit-rate size      : video bit rate (bits/second)\n"
              "  -O --option opt=val     : set media format option to value\n"
              "  -S --single-step        : video single frame at a time mode\n"
              "  -c --crop               : crop rather than scale if resizing\n"
              "  -m --suppress-marker    : suppress marker bits to decoder"
              "  -M --force-marker       : force marker bits to decoder"
              "  -p --payload-size sz    : Set size of maximum RTP payload for encoded data\n"
              "  -S --simultanoues n     : Number of simultaneous encode/decode threads\n"
              "  -T --statistics         : output statistics files\n"
              "  -C --rate-control       : enable rate control\n"
              "  --count n               : set number of frames to transcode\n"
              "  --noprompt              : do not prompt for commands, i.e. exit when input closes\n"
              "  --snr                   : calculate signal-to-noise ratio between input and output"
#if PTRACING
              "  -o or --output file     : file name for output of log messages\n"       
              "  -t or --trace           : degree of verbosity in error log (more times for more detail)\n"     
#endif
              "\n"
              "e.g. " << GetFile().GetTitle() << " --grab-device fake --grab-channel 2 GSM-AMR H.264\n\n";
    return;
  }

  unsigned threadCount = args.GetOptionString('S').AsInteger();
  if (threadCount > 0) {
    unsigned i;
    TestThreadInfo ** infos = (TestThreadInfo **)malloc(threadCount * sizeof(TestThreadInfo *));
    for (i = 0; i < threadCount; ++i) {
      cout << "------------------------------" << endl
           << "Starting thread " << i+1 << endl;
      TestThreadInfo * info = new TestThreadInfo(i+1);
      if (!info->audio.Initialise(args) || !info->video.Initialise(args)) {
        cout << "failed" << endl;
        return;
      }
      info->audio.Resume();
      info->video.Resume();
      infos[i] = info;
    }
    cout << endl << "Tests running" << endl;

    cout << "press return to stop" << endl;
    PCaselessString cmd;
    cin >> cmd;

    for (i = 0; i < threadCount; ++i) {
      infos[i]->audio.Stop();
      infos[i]->video.Stop();
    }

    return;
  }

  AudioThread audio(0);
  VideoThread video(0);

  if (!audio.Initialise(args) || !video.Initialise(args))
    return;

#if 0
  if (video.encoder == NULL && audio.encoder == NULL) {
    cout << "Could not identify media formats ";
    for (int i = 0; i < args.GetCount(); ++i)
      cout << args[i] << ' ';
    cout << endl;
    return;
  }
#endif

  audio.Resume();
  video.Resume();

  // command line
  if (args.HasOption("noprompt")) {
    cout << "Waiting for finish" << endl;
    audio.WaitForTermination();
    video.WaitForTermination();
  }
  else {
    for (;;) {

      // display the prompt
      cout << "codectest> " << flush;
      PCaselessString cmd;
      cin >> cmd;

      if (cmd == "q" || cmd == "x" || cmd == "quit" || cmd == "exit")
        break;

      if (cmd.NumCompare("n") == EqualTo) {
        int steps = cmd.Mid(1).AsUnsigned();
        do {
          video.frameWait.Signal();
        } while (--steps > 0);
        continue;
      }

      if (cmd == "vfu") {
        if (video.encoder == NULL)
          cout << "\nNo video encoder running!" << endl;
        else
          video.encoder->ExecuteCommand(OpalVideoUpdatePicture());
        continue;
      }

      if (cmd == "fg") {
        if (video.grabber == NULL)
          cout << "\nNo video grabber running!" << endl;
        else if (!video.grabber->SetVFlipState(!video.grabber->GetVFlipState()))
          cout << "\nCould not toggle Vflip state of video grabber device" << endl;
        continue;
      }

      if (cmd == "fd") {
        if (video.display == NULL)
          cout << "\nNo video display running!" << endl;
        else if (!video.display->SetVFlipState(!video.display->GetVFlipState()))
          cout << "\nCould not toggle Vflip state of video display device" << endl;
        continue;
      }

      unsigned width, height;
      if (PVideoFrameInfo::ParseSize(cmd, width, height)) {
        video.pause.Signal();
        if (video.grabber == NULL)
          cout << "\nNo video grabber running!" << endl;
        else if (!video.grabber->SetFrameSizeConverter(width, height))
          cout << "Video grabber device could not be set to size " << width << 'x' << height << endl;
        if (video.display == NULL)
          cout << "\nNo video display running!" << endl;
        else if (!video.display->SetFrameSizeConverter(width, height))
          cout << "Video display device could not be set to size " << width << 'x' << height << endl;
        video.resume.Signal();
        continue;
      }

      cout << "Select:\n"
              "  vfu    : Video Fast Update (force I-Frame)\n"
              "  fg     : Flip video grabber top to bottom\n"
              "  fd     : Flip video display top to bottom\n"
              "  qcif   : Set size of grab & display to qcif\n"
              "  cif    : Set size of grab & display to cif\n"
              "  WxH    : Set size of grab & display W by H\n"
              "  N      : Next video frame in sinlge step mode\n"
              "  Q or X : Exit program\n" << endl;
    } // end for

    cout << "Exiting." << endl;
    audio.Stop();
    video.Stop();
  }
}


int TranscoderThread::InitialiseCodec(PArgList & args, const OpalMediaFormat & rawFormat)
{
  if (args.HasOption('m'))
    markerHandling = SuppressMarkers;
  else if (args.HasOption('M'))
    markerHandling = ForceMarkers;

  framesToTranscode = -1;
  PString s = args.GetOptionString("count");
  if (!s.IsEmpty())
    framesToTranscode = s.AsInteger();

  calcSNR = args.HasOption("snr");

  for (PINDEX i = 0; i < args.GetCount(); i++) {
    OpalMediaFormat mediaFormat = args[i];
    if (mediaFormat.IsEmpty()) {
      cout << "Unknown media format name \"" << args[i] << '"' << endl;
      return 0;
    }

    if (mediaFormat.GetMediaType() == rawFormat.GetMediaType()) {
      if (rawFormat == mediaFormat) {
        decoder = NULL;
        encoder = NULL;
      }
      else {
        OpalMediaFormat adjustedRawFormat = rawFormat;
        if (rawFormat == OpalPCM16) {
          if (mediaFormat.GetClockRate() != rawFormat.GetClockRate() || mediaFormat.GetPayloadType() == RTP_DataFrame::G722)
            adjustedRawFormat = OpalPCM16_16KHZ;
          if (args.HasOption('F')) {
            unsigned fpp = args.GetOptionString('F').AsUnsigned();
            if (fpp > 0)
              mediaFormat.SetOptionInteger(OpalAudioFormat::TxFramesPerPacketOption(), fpp);
          }
        }

        if ((encoder = OpalTranscoder::Create(adjustedRawFormat, mediaFormat)) == NULL) {
          cout << "Could not create encoder for media format \"" << mediaFormat << '"' << endl;
          return false;
        }

        if ((decoder = OpalTranscoder::Create(mediaFormat, adjustedRawFormat)) == NULL) {
          cout << "Could not create decoder for media format \"" << mediaFormat << '"' << endl;
          return false;
        }
      }

      return -1;
    }
  }

  return 1;
}


void SetOptions(PArgList & args, OpalMediaFormat & mediaFormat)
{
  PStringArray options = args.GetOptionString('O').Lines();
  for (PINDEX opt = 0; opt < options.GetSize(); ++opt) {
    PString option = options[opt];
    PINDEX equal = option.Find('=');
    if (equal == P_MAX_INDEX)
      cerr << "Illegal option \"" << option << "\" used." << endl;
    else {
      PString name = option.Left(equal);
      PString value = option.Mid(equal+1);
      if (mediaFormat.SetOptionValue(name, value))
        cout << "Option \"" << name << "\" set to \"" << value << "\"." << endl;
      else
        cerr << "Could not set option \"" << name << "\" to \"" << value << "\"." << endl;
    }
  }

  mediaFormat.ToCustomisedOptions();
}

static int udiff(unsigned int const subtrahend, unsigned int const subtractor) 
{
  return subtrahend - subtractor;
}


static double square(double const arg) 
{
  return(arg*arg);
}

static double CalcSNR(const BYTE * src1, const BYTE * src2, PINDEX dataLen)
{
  double diff2 = 0.0;
  for (PINDEX i = 0; i < dataLen; ++i) 
    diff2 += square(udiff(*src1++, *src2++));

  double const snr = diff2 / dataLen / 255;

  return snr;
}

bool AudioThread::Initialise(PArgList & args)
{
  switch (InitialiseCodec(args, OpalPCM16)) {
    case 0 :
      return false;
    case 1 :
      return true;
  }

  readSize = encoder != NULL ? encoder->GetOptimalDataFrameSize(TRUE) : 480;
  OpalMediaFormat mediaFormat = encoder != NULL ? encoder->GetOutputFormat() : OpalPCM16;

  cout << "Audio media format set to " << mediaFormat << endl;

  // Audio recorder
  PString driverName = args.GetOptionString("record-driver");
  PString deviceName = args.GetOptionString("record-device");
  recorder = PSoundChannel::CreateOpenedChannel(driverName, deviceName, PSoundChannel::Recorder, 1, mediaFormat.GetClockRate());
  if (recorder == NULL) {
    cerr << "Cannot use ";
    if (driverName.IsEmpty() && deviceName.IsEmpty())
      cerr << "default ";
    cerr << "audio recorder";
    if (!driverName)
      cerr << ", driver \"" << driverName << '"';
    if (!deviceName)
      cerr << ", device \"" << deviceName << '"';
    cerr << ", must be one of:\n";
    PStringList devices = PSoundChannel::GetDriversDeviceNames("*", PSoundChannel::Recorder);
    for (PINDEX i = 0; i < devices.GetSize(); i++)
      cerr << "   " << devices[i] << '\n';
    cerr << endl;
    return false;
  }

  unsigned bufferCount = args.GetOptionString("record-buffers", "8").AsUnsigned();

  cout << "Audio Recorder ";
  if (!driverName.IsEmpty())
    cout << "driver \"" << driverName << "\" and ";
  cout << "device \"" << recorder->GetName() << "\" using "
       << bufferCount << 'x' << readSize << " byte buffers ";

  if (!recorder->SetBuffers(readSize, bufferCount)) {
    cout << "could not be set." << endl;
    return false;
  }

  cout << "opened and initialised." << endl;

  // Audio player
  driverName = args.GetOptionString("play-driver");
  deviceName = args.GetOptionString("play-device");
  player = PSoundChannel::CreateOpenedChannel(driverName, deviceName, PSoundChannel::Player);
  if (player == NULL) {
    cerr << "Cannot use ";
    if (driverName.IsEmpty() && deviceName.IsEmpty())
      cerr << "default ";
    cerr << "audio player";
    if (!driverName)
      cerr << ", driver \"" << driverName << '"';
    if (!deviceName)
      cerr << ", device \"" << deviceName << '"';
    cerr << ", must be one of:\n";
    PStringList devices = PSoundChannel::GetDriversDeviceNames("*", PSoundChannel::Player);
    for (PINDEX i = 0; i < devices.GetSize(); i++)
      cerr << "   " << devices[i] << '\n';
    cerr << endl;
    return false;
  }

  bufferCount = args.GetOptionString("play-buffers", "8").AsUnsigned();

  cout << "Audio Player ";
  if (!driverName.IsEmpty())
    cout << "driver \"" << driverName << "\" and ";
  cout << "device \"" << player->GetName() << "\" using "
       << bufferCount << 'x' << readSize << " byte buffers ";

  if (!player->SetBuffers(readSize, bufferCount)) {
    cout << "could not be set." << endl;
    return false;
  }

  if (encoder == NULL) 
    frameTime = mediaFormat.GetFrameTime();
  else {
    OpalMediaFormat mediaFormat = encoder->GetOutputFormat();
    SetOptions(args, mediaFormat);
    encoder->UpdateMediaFormats(OpalMediaFormat(), mediaFormat);
  }

  cout << "opened and initialised." << endl;

  return true;
}


bool VideoThread::Initialise(PArgList & args)
{
  switch (InitialiseCodec(args, OpalYUV420P)) {
    case 0 :
      return false;
    case 1 :
      return true;
  }

  OpalMediaFormat mediaFormat = encoder != NULL ? encoder->GetOutputFormat() : OpalYUV420P;

  cout << "Video media format set to " << mediaFormat << endl;

  // Video grabber
  PString driverName = args.GetOptionString("grab-driver");
  PString deviceName = args.GetOptionString("grab-device");
  grabber = PVideoInputDevice::CreateOpenedDevice(driverName, deviceName, FALSE);
  if (grabber == NULL) {
    cerr << "Cannot use ";
    if (driverName.IsEmpty() && deviceName.IsEmpty())
      cerr << "default ";
    cerr << "video grab";
    if (!driverName)
      cerr << ", driver \"" << driverName << '"';
    if (!deviceName)
      cerr << ", device \"" << deviceName << '"';
    cerr << ", must be one of:\n";
    PStringList devices = PVideoInputDevice::GetDriversDeviceNames("*");
    for (PINDEX i = 0; i < devices.GetSize(); i++)
      cerr << "   " << devices[i] << '\n';
    cerr << endl;
    return false;
  }

  cout << "Video Grabber ";
  if (!driverName.IsEmpty())
    cout << "driver \"" << driverName << "\" and ";
  cout << "device \"" << grabber->GetDeviceName() << "\" opened." << endl;


  if (args.HasOption("grab-format")) {
    PVideoDevice::VideoFormat format;
    PCaselessString formatString = args.GetOptionString("grab-format");
    if (formatString == "PAL")
      format = PVideoDevice::PAL;
    else if (formatString == "NTSC")
      format = PVideoDevice::NTSC;
    else if (formatString == "SECAM")
      format = PVideoDevice::SECAM;
    else if (formatString == "Auto")
      format = PVideoDevice::Auto;
    else {
      cerr << "Illegal video grabber format name \"" << formatString << '"' << endl;
      return false;
    }
    if (!grabber->SetVideoFormat(format)) {
      cerr << "Video grabber device could not be set to input format \"" << formatString << '"' << endl;
      return false;
    }
  }
  cout << "Grabber input format set to " << grabber->GetVideoFormat() << endl;

  if (args.HasOption("grab-channel")) {
    int videoInput = args.GetOptionString("grab-channel").AsInteger();
    if (!grabber->SetChannel(videoInput)) {
      cerr << "Video grabber device could not be set to channel " << videoInput << endl;
      return false;
    }
  }
  cout << "Grabber channel set to " << grabber->GetChannel() << endl;

  
  if (args.HasOption("frame-rate"))
    frameRate = args.GetOptionString("frame-rate").AsUnsigned();
  else
    frameRate = grabber->GetFrameRate();

  mediaFormat.SetOptionInteger(OpalVideoFormat::FrameTimeOption(), mediaFormat.GetClockRate()/frameRate);

  if (!grabber->SetFrameRate(frameRate)) {
    cerr << "Video grabber device could not be set to frame rate " << frameRate << endl;
    return false;
  }

  cout << "Grabber frame rate set to " << grabber->GetFrameRate() << endl;

  // Video display
  driverName = args.GetOptionString("display-driver");
  deviceName = args.GetOptionString("display-device");
  display = PVideoOutputDevice::CreateOpenedDevice(driverName, deviceName, FALSE);
  if (display == NULL) {
    cerr << "Cannot use ";
    if (driverName.IsEmpty() && deviceName.IsEmpty())
      cerr << "default ";
    cerr << "video display";
    if (!driverName)
      cerr << ", driver \"" << driverName << '"';
    if (!deviceName)
      cerr << ", device \"" << deviceName << '"';
    cerr << ", must be one of:\n";
    PStringList devices = PVideoOutputDevice::GetDriversDeviceNames("*");
    for (PINDEX i = 0; i < devices.GetSize(); i++)
      cerr << "   " << devices[i] << '\n';
    cerr << endl;
    return false;
  }

  cout << "Display ";
  if (!driverName.IsEmpty())
    cout << "driver \"" << driverName << "\" and ";
  cout << "device \"" << display->GetDeviceName() << "\" opened." << endl;

  // Configure sizes/speeds
  unsigned width, height;
  if (args.HasOption("frame-size")) {
    PString sizeString = args.GetOptionString("frame-size");
    if (!PVideoFrameInfo::ParseSize(sizeString, width, height)) {
      cerr << "Illegal video frame size \"" << sizeString << '"' << endl;
      return false;
    }
  }
  else
    grabber->GetFrameSize(width, height);

  mediaFormat.SetOptionInteger(OpalVideoFormat::FrameWidthOption(), width);
  mediaFormat.SetOptionInteger(OpalVideoFormat::FrameHeightOption(), height);

  PVideoFrameInfo::ResizeMode resizeMode = args.HasOption("crop") ? PVideoFrameInfo::eCropCentre : PVideoFrameInfo::eScale;
  if (!grabber->SetFrameSizeConverter(width, height, resizeMode)) {
    cerr << "Video grabber device could not be set to size " << width << 'x' << height << endl;
    return false;
  }
  cout << "Grabber frame size set to " << grabber->GetFrameWidth() << 'x' << grabber->GetFrameHeight() << endl;

  if  (!display->SetFrameSizeConverter(width, height, resizeMode)) {
    cerr << "Video display device could not be set to size " << width << 'x' << height << endl;
    return false;
  }

  cout << "Display frame size set to " << display->GetFrameWidth() << 'x' << display->GetFrameHeight() << endl;

  if (!grabber->SetColourFormatConverter("YUV420P") ) {
    cerr << "Video grabber device could not be set to colour format YUV420P" << endl;
    return false;
  }

  cout << "Grabber colour format set to " << grabber->GetColourFormat() << " (";
  if (grabber->GetColourFormat() == "YUV420P")
    cout << "native";
  else
    cout << "converted to YUV420P";
  cout << ')' << endl;

  if (!display->SetColourFormatConverter("YUV420P")) {
    cerr << "Video display device could not be set to colour format YUV420P" << endl;
    return false;
  }

  cout << "Display colour format set to " << display->GetColourFormat() << " (";
  if (display->GetColourFormat() == "YUV420P")
    cout << "native";
  else
    cout << "converted from YUV420P";
  cout << ')' << endl;


  if (args.HasOption("bit-rate")) {
    PString str = args.GetOptionString("bit-rate");
    double bitrate = str.AsReal();
    switch (str[str.GetLength()-1]) {
      case 'K' :
      case 'k' :
        bitrate *= 1000;
        break;
      case 'M' :
      case 'm' :
        bitrate *= 1000000;
        break;
    }
    if (bitrate < 100 || bitrate > INT_MAX) {
      cerr << "Could not set bit rate to " << str << endl;
      return false;
    }
    mediaFormat.SetOptionInteger(OpalVideoFormat::TargetBitRateOption(), (unsigned)bitrate);
    if (mediaFormat.GetOptionInteger(OpalVideoFormat::MaxBitRateOption()) < bitrate)
      mediaFormat.SetOptionInteger(OpalVideoFormat::MaxBitRateOption(), (unsigned)bitrate);
  }
  cout << "Target bit rate set to " << mediaFormat.GetOptionInteger(OpalVideoFormat::TargetBitRateOption()) << " bps" << endl;

  if (args.HasOption('T')) {
    frameFn  = "frame_stats.csv";
    packetFn = "packet_stats.csv";
  }

  if (args.HasOption('C')) {
    rcEnable = true;
    unsigned rate = mediaFormat.GetOptionInteger(OpalVideoFormat::TargetBitRateOption());
    rateController.Open(rate);
    cout << "Video rate controller enabled for bit rate " << rate << " bps" << endl;
  }

  SetOptions(args, mediaFormat);

  if (encoder == NULL) 
    frameTime = mediaFormat.GetFrameTime();
  else {
    encoder->UpdateMediaFormats(OpalMediaFormat(), mediaFormat);
    if (args.HasOption('p'))
      encoder->SetMaxOutputSize(args.GetOptionString('p').AsUnsigned());
  }

  singleStep = args.HasOption('S');

  sumYSNR = sumCbSNR = sumCrSNR = 0.0;
  snrCount = 0;

  return true;
}


void AudioThread::Main()
{
  if (recorder == NULL || player == NULL) {
    cerr << "Audio cannot open recorder or player" << endl;
    return;
  }

  TranscoderThread::Main();
}


void VideoThread::Main()
{
  if (grabber == NULL || display == NULL) {
    cerr << "Video cannot open grabber or display" << endl;
    return;
  }

  grabber->Start();
  display->Start();

  //decoder->SetCommandNotifier(PCREATE_NOTIFIER(OnTranscoderCommand));

  forceIFrame = false;
  TranscoderThread::Main();
}

void TranscoderThread::OnTranscoderCommand(OpalMediaCommand & cmd, INT)
{
  if (PIsDescendant(&cmd, OpalVideoUpdatePicture)) {
    cout << "decoder lost sync" << endl;
    forceIFrame = true;
    ((OpalVideoTranscoder *)encoder)->ForceIFrame();
  }
  else {
    cout << "unknown decoder command " << cmd.GetName() << endl;
  }
}

void VideoThread::CalcSNR(const RTP_DataFrame & src, const RTP_DataFrame & dst)
{
  if (src.GetPayloadSize() < (PINDEX)sizeof(OpalVideoTranscoder::FrameHeader) ||
      dst.GetPayloadSize() < (PINDEX)sizeof(OpalVideoTranscoder::FrameHeader))
    return;

  const BYTE * src1 = src.GetPayloadPtr();
  const BYTE * src2 = dst.GetPayloadPtr();

  const OpalVideoTranscoder::FrameHeader * hdr1 = (OpalVideoTranscoder::FrameHeader *)src1;
  const OpalVideoTranscoder::FrameHeader * hdr2 = (OpalVideoTranscoder::FrameHeader *)src2;

  if (hdr1->height != hdr2->height || hdr1->width != hdr2->width)
    return;

  if (hdr1->height != snrHeight || hdr1->width != snrWidth) {
    sumYSNR = sumCbSNR = sumCrSNR = 0.0;
    snrCount = 0;
    snrHeight = hdr1->height;
    snrWidth  = hdr1->width;
  }

  unsigned size = snrHeight * snrWidth;

  int tsize = sizeof(OpalVideoTranscoder::FrameHeader) + size*3/2;

  if (src.GetPayloadSize() < tsize || dst.GetPayloadSize() < tsize)
    return;

  src1 += sizeof(OpalVideoTranscoder::FrameHeader);
  src2 += sizeof(OpalVideoTranscoder::FrameHeader);

  sumYSNR  = ::CalcSNR(src1, src2, size);
  src1 += size;
  src2 += size;

  size = size / 4;
  sumCbSNR = ::CalcSNR(src1, src2, size);
  src1 += size;
  src2 += size;

  sumCrSNR = ::CalcSNR(src1, src2, size);

  ++snrCount;
}

void VideoThread::ReportSNR()
{
  /* The PSNR is the mean of the sum of squares of the differences,
     normalized to the range 0..1
  */
  double const yPsnr = sumYSNR / snrCount;

  if (yPsnr <= 1e-9)
    cout << "Y  color component identical" << endl;
  else
    printf("Y  color component avg SNR : %.2f dB\n", 10 * log10(1/yPsnr));

  double const cbPsnr = sumCbSNR / snrCount;

  if (cbPsnr <= 1e-9)
    cout << "Cb color component identical" << endl;
  else
    printf("Cb color component avg SNR : %.2f dB\n", 10 * log10(1/cbPsnr));

  double const crPsnr = sumCrSNR / snrCount;

  if (crPsnr <= 1e-9)
    cout << "Cr color component identical" << endl;
  else
    printf("Cr color component avg SNR : %.2f dB\n", 10 * log10(1/crPsnr));
}


void TranscoderThread::Main()
{
  PUInt64 byteCount = 0;
  PUInt64 frameCount = 0;
  PUInt64 packetCount = 0;
  WORD sequenceNumber = 0;
  bool oldSrcState = true;
  bool oldOutState = true;
  bool oldEncState = true;
  bool oldDecState = true;

  PTimeInterval startTick = PTimer::Tick();
  while (running && framesToTranscode < 0 || (framesToTranscode-- > 0)) {

    RTP_DataFrame srcFrame;
    bool state = Read(srcFrame);
    if (oldSrcState != state) {
      oldSrcState = state;
      cerr << "Source " << (state ? "restor" : "fail") << "ed at frame " << frameCount << endl;
    }

    srcFrame.SetTimestamp(timestamp);
    timestamp += frameTime;

    if (rcEnable && rateController.SkipFrame()) {
      cerr << "Rate controller forced frame skip" << endl;
      continue;
    }
      
    RTP_DataFrameList encFrames;
    if (encoder == NULL) 
      encFrames.Append(new RTP_DataFrame(srcFrame)); 
    else {
      state = encoder->ConvertFrames(srcFrame, encFrames);
      if (oldEncState != state) {
        oldEncState = state;
        cerr << "Encoder " << (state ? "restor" : "fail") << "ed at frame " << frameCount << endl;
        continue;
      }
    }

    for (PINDEX i = 0; i < encFrames.GetSize(); i++) {
      encFrames[i].SetSequenceNumber(++sequenceNumber);
    }

    unsigned long frameSize = 0;
    unsigned long framePacketCount = 0;
    for (PINDEX i = 0; i < encFrames.GetSize(); i++) {
      RTP_DataFrameList outFrames;
      if (encoder == NULL)
        outFrames = encFrames;
      else {
        switch (markerHandling) {
          case SuppressMarkers :
            encFrames[i].SetMarker(false);
            break;
          case ForceMarkers :
            encFrames[i].SetMarker(true);
            break;
          default :
            break;
        }

        state = decoder->ConvertFrames(encFrames[i], outFrames);

        if (oldDecState != state) {
          oldDecState = state;
          cerr << "Decoder " << (state ? "restor" : "fail") << "ed at packet " << packetCount << endl;
          continue;
        }
        UpdateStats(encFrames[i]);
      }

      for (PINDEX j = 0; j < outFrames.GetSize(); j++) {

        if (calcSNR) 
          CalcSNR(srcFrame, outFrames[j]);

        state = Write(outFrames[j]);
        if (oldOutState != state) {
          oldOutState = state;
          cerr << "Frame display " << (state ? "restor" : "fail") << "ed at packet " << packetCount << endl;
        }
      }
      byteCount += encFrames[i].GetPayloadSize();
      frameSize += encFrames[i].GetPayloadSize() + encFrames[i].GetHeaderSize();
      framePacketCount++;
      packetCount++;
    }


    if (rcEnable)
      rateController.AddFrame(frameSize, framePacketCount);

    if (pause.Wait(0)) {
      pause.Acknowledge();
      resume.Wait();
    }

    frameCount++;
  }

  PTimeInterval duration = PTimer::Tick() - startTick;

  cout << fixed << setprecision(1);
  if (byteCount < 10000ULL)
    cout << byteCount << ' ';
  else if (byteCount < 10000000ULL)
    cout << byteCount/1000.0 << " k";
  else if (byteCount < 10000000000ULL)
    cout << byteCount/1000000.0 << " M";
  cout << "bytes, "
       << packetCount << " packets, "
       << frameCount << " frames over " << duration << " seconds at "
       << (frameCount*1000.0/duration.GetMilliSeconds()) << " f/s and ";

  PUInt64 bitRate = byteCount*8/duration.GetSeconds();
  if (bitRate < 10000ULL)
    cout << bitRate << ' ';
  else if (bitRate < 10000000ULL)
    cout << bitRate/1000.0 << " k";
  else if (bitRate < 10000000000ULL)
    cout << bitRate/1000000.0 << " M";
  cout << "bits/s." << endl;

  if (calcSNR) 
    ReportSNR();
}


bool AudioThread::Read(RTP_DataFrame & frame)
{
  frame.SetPayloadSize(readSize);
  return recorder->Read(frame.GetPayloadPtr(), readSize);
}


bool AudioThread::Write(const RTP_DataFrame & frame)
{
  return player->Write(frame.GetPayloadPtr(), frame.GetPayloadSize());
}


void AudioThread::Stop()
{
  running = false;
  WaitForTermination();
}


bool VideoThread::Read(RTP_DataFrame & data)
{
  if (singleStep)
    frameWait.Wait();

  data.SetPayloadSize(grabber->GetMaxFrameBytes()+sizeof(OpalVideoTranscoder::FrameHeader));
  data.SetMarker(TRUE);

  unsigned width, height;
  grabber->GetFrameSize(width, height);
  OpalVideoTranscoder::FrameHeader * frame = (OpalVideoTranscoder::FrameHeader *)data.GetPayloadPtr();
  frame->x = frame->y = 0;
  frame->width = width;
  frame->height = height;

  return grabber->GetFrameData(OPAL_VIDEO_FRAME_DATA_PTR(frame));
}


bool VideoThread::Write(const RTP_DataFrame & data)
{
  const OpalVideoTranscoder::FrameHeader * frame = (const OpalVideoTranscoder::FrameHeader *)data.GetPayloadPtr();
  display->SetFrameSize(frame->width, frame->height);
  return display->SetFrameData(frame->x, frame->y,
                               frame->width, frame->height,
                               OPAL_VIDEO_FRAME_DATA_PTR(frame), data.GetMarker());
}


void VideoThread::Stop()
{
  running = false;
  frameWait.Signal();
  WaitForTermination();
}

void VideoThread::InitStats()
{
  packetCount= 0;
  frameCount = 0;
  frameBytes = 0;
  totalFrameBytes = 0;
}

void VideoThread::UpdateStats(const RTP_DataFrame & frame)
{
  if (!packetFn.IsEmpty()) {
    if (packetStatFile.Open(packetFn, PFile::WriteOnly))
      packetStatFile << "PacketNum,PacketSize\n";
    packetFn.MakeEmpty();
  }

  packetCount++;
  frameBytes      += frame.GetPayloadSize();
  totalFrameBytes += frame.GetPayloadSize();

  if (packetStatFile.IsOpen()) 
    packetStatFile << packetCount << ','
                   << frame.GetPayloadSize() << endl;

  if (frame.GetMarker()) 
    UpdateFrameStats();
}

PInt64 BpsTokbps(PInt64 Bps)
{
  return ((Bps * 8) + 500) / 1000;
}

void VideoThread::UpdateFrameStats()
{
  if (!frameFn.IsEmpty()) {
    if (frameStatFile.Open(frameFn, PFile::WriteOnly))
      frameStatFile << "FrameNum,FrameBits,FrameKBPS,TotalKBPS\n";
    frameFn.MakeEmpty();
  }

  frameCount++;

  if (frameStatFile.IsOpen()) {
    frameStatFile << frameCount << ','
                  << (frameBytes * 8) << ','
                  << BpsTokbps(frameBytes * frameRate) << ','
                  << BpsTokbps((totalFrameBytes * frameRate) / frameCount) << endl;
  }

  frameBytes = 0;
}

// End of File ///////////////////////////////////////////////////////////////
