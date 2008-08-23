/*
 *
 * Inter Asterisk Exchange 2
 * 
 * Implementation of the class to receive all packets for all calls.
 * 
 * Open Phone Abstraction Library (OPAL)
 *
 * Copyright (c) 2005 Indranet Technologies Ltd.
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
 * The Original Code is Open Phone Abstraction Library.
 *
 * The Initial Developer of the Original Code is Indranet Technologies Ltd.
 *
 * The author of this code is Derek J Smithies
 *
 *
 *  $Log: receiver.cxx,v $
 *  Revision 1.7  2007/01/11 03:02:16  dereksmithies
 *  Remove the previous audio buffering code, and switch to using the jitter
 *  buffer provided in Opal. Reduce the verbosity of the log mesasges.
 *
 *  Revision 1.6  2007/01/09 03:32:23  dereksmithies
 *  Tidy up and improve the close down process - make it more robust.
 *  Alter level of several PTRACE statements. Add Terminate() method to transmitter and receiver.
 *
 *  Revision 1.5  2007/01/08 04:06:58  dereksmithies
 *  Modify logging level, and improve the comments.
 *
 *  Revision 1.4  2006/08/09 03:46:39  dereksmithies
 *  Add ability to register to a remote Asterisk box. The iaxProcessor class is split
 *  into a callProcessor and a regProcessor class.
 *  Big thanks to Stephen Cook, (sitiveni@gmail.com) for this work.
 *
 *  Revision 1.3  2005/08/26 03:07:38  dereksmithies
 *  Change naming convention, so all class names contain the string "IAX2"
 *
 *  Revision 1.2  2005/08/24 01:38:38  dereksmithies
 *  Add encryption, iax2 style. Numerous tidy ups. Use the label iax2, not iax
 *
 *  Revision 1.1  2005/07/30 07:01:33  csoutheren
 *  Added implementation of IAX2 (Inter Asterisk Exchange 2) protocol
 *  Thanks to Derek Smithies of Indranet Technologies Ltd. for
 *  writing and contributing this code
 *
 *
 *
 *
 */

#include <ptlib.h>
#include <opal/buildopts.h>

#ifdef P_USE_PRAGMA
#pragma implementation "receiver.h"
#endif

#include <iax2/receiver.h>
#include <iax2/iax2ep.h>

#define new PNEW

IAX2Receiver::IAX2Receiver(IAX2EndPoint & _newEndpoint, PUDPSocket & _newSocket)
  : PThread(1000, NoAutoDeleteThread, NormalPriority, "IAX Receiver"),
     endpoint(_newEndpoint),
     sock(_newSocket)
{
  keepGoing = PTrue;
  fromNetworkFrames.Initialise();
  
  PTRACE(6, "IAX2 Rx\tReceiver Constructed just fine");
  PTRACE(6, "IAX2 Rx\tListen on socket " << sock);
  Resume();
}

IAX2Receiver::~IAX2Receiver()
{
  Terminate();
  WaitForTermination();
  
  fromNetworkFrames.AllowDeleteObjects();

  PTRACE(6, "IAX2 Rx\tDestructor finished");

}

void IAX2Receiver::Terminate()
{
  PTRACE(5, "IAX2 Rx\tEnd receiver thread");
  keepGoing = PFalse;
  
  PIPSocket::Address addr;
  sock.GetLocalAddress(addr);
  sock.WriteTo("", 1, addr, sock.GetPort());
  sock.Close();   //This kills the reading of packets from the socket, and activates receiver thread.
}

void IAX2Receiver::Main()
{
  SetThreadName("IAX2Receiver");
  
  while (keepGoing) {
    PBoolean res = ReadNetworkSocket();
    
    if ((res == PFalse) || (keepGoing == PFalse)) {
      PTRACE(3, "IAX2 Rx\tNetwork socket has closed down, so exit");
      break;            /*Network socket has closed down*/
    }
    PTRACE(6, "IAX2 Rx\tHave successfully read a packet from the network");
    
    for(;;) {
      IAX2Frame *frame     = (IAX2Frame *)fromNetworkFrames.GetLastFrame();
      if (frame == NULL)
        break;
      
      endpoint.IncomingEthernetFrame(frame);
    }
  }
  PTRACE(4, "IAX2 Rx\tEnd of IAX2 receiver thread ");
}


void IAX2Receiver::AddNewReceivedFrame(IAX2Frame *newFrame)
{
  /**This method may split a frame up (if it is trunked) */
  PTRACE(6, "IAX2 Rx\tAdd frame to list of received frames " << newFrame->IdString());
  fromNetworkFrames.AddNewFrame(newFrame);
}

PBoolean IAX2Receiver::ReadNetworkSocket()
{
  IAX2Frame *frame = new IAX2Frame(endpoint);
  
  PTRACE(5, "IAX2 Rx\tWait for packet on socket.with port " << sock.GetPort() << " FrameID-->" << frame->IdString());
  PBoolean res = frame->ReadNetworkPacket(sock);
  
  if (res == PFalse) {
    PTRACE(3, "IAX2 Rx\tFailed to read network packet from socket for FrameID-->" << frame->IdString());
    delete frame;
    return PFalse;
  }
  
  PTRACE(6, "IAX2 Rx\tHave read a frame from the network socket fro FrameID-->" 
	 << frame->IdString() << endl  << *frame);
  
  if(frame->ProcessNetworkPacket() == PFalse) {
    PTRACE(3, "IAX2 Rx\tFailed to interpret header for " << frame->IdString());
    delete frame;
    return PTrue;
  }
  
  /* At this point, the IAX2Connection instance this frame belongs to is
     known, and stored in the frame structure. Consequently, the frame is
     ready to be passed on to the endpoint for distribution, and is passed on
     to the endpoint.IncomingEthernetFrame method. 

     We use the "fromNetworkFrames" list as a buffer, to ensure the receive
     thread is not held up in the distribution of this frame. Consequently,
     the chance of dropping a frame is less.*/
  AddNewReceivedFrame(frame);
  
  return PTrue;
}
/* The comment below is magic for those who use emacs to edit this file. */
/* With the comment below, the tab key does auto indent to 4 spaces.     */

/*
 * Local Variables:
 * mode:c
 * c-file-style:linux
 * c-basic-offset:2
 * End:
 */

