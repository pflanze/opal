/*p64encoder.h copyright (c)Indranet Technologies ltd (lara@indranet.co.nz)
 *                        Author Derek J Smithies (derek@indranet.co.nz)
 *
 *
 * This file defines the p64encoder class, which is the combined total of
 * the grabber, pre encoder, and encoder classes.
 */

/************ Change log
 *
 * $Log: p64encoder.h,v $
 * Revision 1.2001  2001/07/27 15:48:25  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.12  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.11  2001/03/19 01:41:33  robertj
 * Removed last vestiges of old grabber code.
 *
 * Revision 1.10  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.9  2000/10/13 01:47:27  dereks
 * Include command option for setting the number of transmitted video
 * frames per second.   use --videotxfps n
 *
 * Revision 1.8  2000/09/08 06:41:38  craigs
 * Added ability to set video device
 * Added ability to select test input frames
 *
 * Revision 1.7  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#ifndef lib_p64encoder_h
#define lib_p64encoder_h

#include "config.h"
#include "p64.h"
#include "vid_coder.h"
#include "videoframe.h"
#include "encoder-h261.h"
#include "transmitter.h"

#define WIDTH 352
#define HEIGHT 288



class P64Encoder{
    public:
       /**Constructor for using a grabber. The grabber may elect to read data from a 
          hardware device, or from data in memory, Data in memory is placed there by 
          programs such as openmcu.
	*/
      P64Encoder(int quant_level,int idle);

      ~P64Encoder();
      
      /**Set size of the grabbing window, which sets the
         size of the vid_frame class. When the vid_frame class
         is passed to grabber and encoder, the grabber and
         encoder automatically resize */
      void SetSize(int width,int height);

      /**Called by the display routine, so we can display
         local video using the grabbed 411 format stuff.  
         Returns the address of the grabbed 411 format frame.
      */
      u_char* GetFramePtr();

      //        /**Grabs one video frame,
      //     or generates test pattern if grabber is not operational */
      //    void GrabOneFrame();


        /** On a previously grabbed frame,
            1)do some motion and block changed checking,
            2)h261 format conversion and
            3)store result in a series of packets in a list 
	         in the transmitter class.  */
	void ProcessOneFrame();

        /**Retrieves the first packet in the list recorded in
	       in the transmitter class */
        void ReadOnePacket(
                   u_char * buffer,    /// Buffer of encoded data
                   unsigned & length /// Actual length of encoded data buffer
		   );
 
	/**Return the number of packets waiting to be transmitted. These
           packets were created at the last invocation of the video codec.
	*/ 
        int PacketsOutStanding()
	  { return trans->PacketsOutStanding();}

protected:
   //variables used in grabbing/processing the image. 
    Transmitter      *trans;
    H261PixelEncoder *h261_edr;
    VideoFrame       *vid_frame;
    Pre_Vid_Coder    *pre_vid;

};


#endif   //#ifndef lib_p64encoder_h
