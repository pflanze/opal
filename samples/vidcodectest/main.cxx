#include <ptlib.h>

#include <ptclib/pvfiledev.h>
#include <opal/transcoders.h>
#include <codec/vidcodec.h>

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define BUILD_TYPE    ReleaseCode
#define BUILD_NUMBER  0

class VidCodecTest : public PProcess
{
  PCLASSINFO(VidCodecTest, PProcess)

  public:
    VidCodecTest();

    void Main();
};

PCREATE_PROCESS(VidCodecTest);

VidCodecTest::VidCodecTest()
  : PProcess("Post Increment", "VidCodecTest",
             MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}

void ListCodecs()
{
  OpalTranscoderList keys = OpalTranscoderFactory::GetKeyList();
  OpalTranscoderList::const_iterator r;
  for (r = keys.begin(); r != keys.end(); ++r) {
    const OpalMediaFormatPair & transcoder = *r;
    if (transcoder.GetInputFormat().GetDefaultSessionID() == OpalMediaFormat::DefaultVideoSessionID) {
      cout << "Name: " << transcoder << "\n"
           << "  Input:  " << transcoder.GetInputFormat() << "\n"
           << "  Output: " << transcoder.GetOutputFormat() << "\n";
    }
  }
}

void VidCodecTest::Main()
{
  cout << GetName()
       << " Version " << GetVersion(TRUE)
       << " by " << GetManufacturer()
       << " on " << GetOSClass() << ' ' << GetOSName()
       << " (" << GetOSVersion() << '-' << GetOSHardware() << ")\n\n";

  PConfigArgs args(GetArguments());

  args.Parse(
#if PTRACING
             "o-output:"
             "t-trace."
#endif
             "h-help."
          , FALSE);

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL);
#endif

  if ((args.GetCount() == 1) && (args[0] *= "list")) {
    ListCodecs();
    return;
  }

  if (args.GetCount() < 4) {
    PError << "usage: h263test 'encode'|'decode'|'xcode' codec infilename outfilename" << endl;
    return;
  }

  char coding = tolower(args[0][0]);

  PString inCodec, outCodec;
  OpalTranscoder * encoder = NULL;
  OpalTranscoder * decoder = NULL;

  BOOL error = FALSE;
  if (coding == 'd' || coding == 'x') {
    inCodec  = args[1];
    outCodec = "YUV420P";
    decoder = OpalTranscoder::Create(inCodec, outCodec);
    if (decoder == NULL) {
      PError << "error: unable to create decoder of " << inCodec << endl;
      error = TRUE;
    }
    else {
      cout << "Created decoder from " << inCodec << " to " << outCodec << endl;
    }
  }
  if (coding == 'e' || coding == 'x') {
    inCodec = "YUV420P";
    outCodec  = args[1];
    encoder = OpalTranscoder::Create(inCodec, outCodec);
    if (encoder == NULL) {
      PError << "error: unable to create encoder of " << outCodec << endl;
      error = TRUE;
    }
    else
      cout << "Created encoder from " << inCodec << " to " << outCodec << endl;
  }
  
  if (error) {
    PError << "Valid transcoders are:";
    ListCodecs();
    return;
  }

  PYUVFile yuvIn;
  PYUVFile yuvOut;
  if (coding == 'e' || coding == 'x') {
    if (!yuvIn.Open(args[2], PFile::ReadOnly, PFile::MustExist)) {
      PError << "error: cannot open YUV input file " << args[2] << endl;
      return;
    }
  }
  if (coding == 'd' || coding == 'x') {
    if (!yuvOut.Open(args[3], PFile::WriteOnly)) {
      PError << "error: cannot open YUV output file " << args[3] << endl;
      return;
    }
  }

  if (coding == 'd') {
    PError << "error: decoding not yet implemented" << endl;
    return;
  }
  if (coding == 'e') {
    PError << "error: encoding not yet implemented" << endl;
    return;
  }
  if (coding == 'x') {

    PINDEX frameCount = 0;

    for (;;) {

      RTP_DataFrame yuvInFrame(sizeof(PluginCodec_Video_FrameHeader) + (yuvIn.GetWidth() * yuvOut.GetHeight() * 3) / 2);
      RTP_DataFrameList encodedFrames, yuvOutFrame;

      PluginCodec_Video_FrameHeader * header = (PluginCodec_Video_FrameHeader *)yuvInFrame.GetPayloadPtr();

      header->x      = 0;
      header->y      = 0;
      header->width  = yuvIn.GetWidth();
      header->height = yuvIn.GetHeight();

      if (!yuvIn.ReadFrame(header->data)) {
        break;
      }

      if (!encoder->ConvertFrames(yuvInFrame, encodedFrames)) {
        PError << "error: encoder returned error" << endl;
        break;
      }

      yuvOutFrame.RemoveAll();
      PINDEX i;
      for (i = 0; i < encodedFrames.GetSize(); ++i) {
        if (!decoder->ConvertFrames(encodedFrames[i], yuvOutFrame)) {
          PError << "error: decoder returned error" << endl;
          break;
        }
        if (yuvOutFrame.GetSize() > 0)
          break;
      }
      if (i != encodedFrames.GetSize()-1) {
        PError << "warning: frame created from incomplete input frame list" << endl;
      }

      PluginCodec_Video_FrameHeader * headerOut = (PluginCodec_Video_FrameHeader *)yuvOutFrame[0].GetPayloadPtr();

      if (!yuvOut.WriteFrame(headerOut->data)) {
        PError << "error: output file write failed" << endl;
        break;
      }

      ++frameCount;
    }

    cout << frameCount << " frames transcoded" << endl;
  }

#if 0

  PYUVFile inFile(176, 144, args[1], PFile::ReadOnly, PFile::MustExist);
  if (!inFile.IsOpen()) {
    PError << "error: cannot open input file " << inFile.GetFilePath() << endl;
    return;
  }

  PVideoChannel      * channel = new PVideoChannel;
  PVideoInputDevice  * grabber = PVideoInputDevice::CreateDevice(VideoGrabberDriverName);
  if (grabber == NULL) {
    PError << "error: cannot create video input device for driver " << VideoGrabberDriverName << endl;
    return;
  }

  if (!InitGrabber(grabber, 176, 144, args[0], 0, 10)) {
    delete grabber;
    return;
  }

  H323_RFC2190_H263Codec encoder(H323VideoCodec::Encoder, 
      1,     // unsigned sqcifMPI = 1,	// {1..3600 units seconds/frame, 1..32 units 1/29.97 Hz}
      2,     // unsigned qcifMPI = 2,
      4,     // unsigned cifMPI = 4,
      8,     // unsigned cif4MPI = 8,
      32,    // unsigned cif16MPI = 32,
      400,   // unsigned maxBitRate = 400,
      FALSE, // BOOL unrestrictedVector = FALSE,
      FALSE, // BOOL arithmeticCoding = FALSE, // not supported
      FALSE, // BOOL advancedPrediction = FALSE,
      FALSE  // BOOL pbFrames = FALSE,
  );

  grabber->Start();
  channel->AttachVideoReader(grabber);

  if (!encoder.AttachChannel(channel, TRUE)) {
    PError << "error: cannot attach grabber to codec" << endl;
    return;
  }

  PFile outFile(args[1], PFile::WriteOnly, PFile::Create);
  if (!outFile.IsOpen()) {
    PError << "error: cannot create output file " << outFile.GetFilePath() << endl;
    return;
  }

  BYTE carryOver = 0;

  for (;;) {
    RTP_DataFrame rtpFrame;
    unsigned length;
    if (!encoder.Read(rtpFrame.GetPayloadPtr(), length, rtpFrame))
      break;
    BYTE * data = rtpFrame.GetPayloadPtr();
    PINDEX size = rtpFrame.GetPayloadSize();
    static unsigned modes[4] = {
        4,   // mode A
        4,   // mode A
        8,   // mode B
        12   // mode C
    };
    PINDEX offset = modes[(data[0] >> 6) & 3];
    PINDEX sbit   = (data[0] >> 3) & 0x7;
    PINDEX ebit   = data[0] & 0x7;
    if (sbit != 0)
      data[offset] |= carryOver;
    if (ebit != 0) {
      carryOver = data[offset + size - 1];
      --size;
    }

    outFile.Write(data + offset, size - offset);
  }

#endif
}

