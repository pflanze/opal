/*
 *
 * Inter Asterisk Exchange 2
 * 
 * Class to implement the transmitter, which sends all packets for all calls.
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
 *  $Log: transmit.cxx,v $
 *  Revision 1.7  2007/08/07 03:10:18  dereksmithies
 *  Modify comments. Reduce some of the verbosity of the PTRACE messages.
 *
 *  Revision 1.6  2007/01/23 02:10:39  dereksmithies
 *   Handle Vnak frames correctly.  Handle iseqno and oseqno correctly.
 *
 *  Revision 1.5  2007/01/11 03:02:16  dereksmithies
 *  Remove the previous audio buffering code, and switch to using the jitter
 *  buffer provided in Opal. Reduce the verbosity of the log mesasges.
 *
 *  Revision 1.4  2007/01/09 03:32:23  dereksmithies
 *  Tidy up and improve the close down process - make it more robust.
 *  Alter level of several PTRACE statements. Add Terminate() method to transmitter and receiver.
 *
 *  Revision 1.3  2006/08/09 03:46:39  dereksmithies
 *  Add ability to register to a remote Asterisk box. The iaxProcessor class is split
 *  into a callProcessor and a regProcessor class.
 *  Big thanks to Stephen Cook, (sitiveni@gmail.com) for this work.
 *
 *  Revision 1.2  2005/08/26 03:07:38  dereksmithies
 *  Change naming convention, so all class names contain the string "IAX2"
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
#pragma implementation "transmit.h"
#endif

#include <iax2/transmit.h>

#define new PNEW

IAX2Transmit::IAX2Transmit(IAX2EndPoint & _newEndpoint, PUDPSocket & _newSocket)
  : PThread(1000, NoAutoDeleteThread, NormalPriority, "IAX Transmitter"),
     ep(_newEndpoint),
     sock(_newSocket)
{
  sendNowFrames.Initialise();
  ackingFrames.Initialise();
  
  keepGoing = PTrue;
  
  PTRACE(6,"Constructor - IAX2 Transmitter");
  Resume();
}

IAX2Transmit::~IAX2Transmit()
{
  Terminate();
  WaitForTermination();
  sendNowFrames.AllowDeleteObjects();
  
  IAX2FrameList notUsed; /* These frames will be destroyed at method end. */
  ackingFrames.GrabContents(notUsed);
  PTRACE(5, "IAX2Transmit\tDestructor finished");
}

void IAX2Transmit::Terminate()
{
  keepGoing = PFalse;
  activate.Signal();
}

void IAX2Transmit::SendFrame(IAX2Frame *newFrame)
{
  sendNowFrames.AddNewFrame(newFrame);
  
  activate.Signal();
}

void IAX2Transmit::PurgeMatchingFullFrames(IAX2Frame *newFrame)
{
  if (!PIsDescendant(newFrame, IAX2FullFrame))
    return;

  PTRACE(5, "PurgeMatchingFullFrames to " << *newFrame);

  ackingFrames.DeleteMatchingSendFrame((IAX2FullFrame *)newFrame);
}

void IAX2Transmit::SendVnakRequestedFrames(IAX2FullFrameProtocol &src)
{
  PTRACE(4, "SendVnakRequestedFramees to " << src);
  ackingFrames.SendVnakRequestedFrames(src);
}


void IAX2Transmit::Main()
{
  SetThreadName("IAX2Transmit");
  while(keepGoing) {
    activate.Wait();
    
    if (!keepGoing)
      break;

    ProcessAckingList();
    
    ProcessSendList();
  }
  PTRACE(6, "IAX2Transmit\tEnd of the Transmit thread.");  
}

void IAX2Transmit::ProcessAckingList()
{
  IAX2ActiveFrameList framesToSend;
  
  PTRACE(5, "GetResendFramesDeleteOldFrames");
  ackingFrames.GetResendFramesDeleteOldFrames(framesToSend);
  
  framesToSend.MarkAllAsResent();
  
  sendNowFrames.GrabContents(framesToSend);
}

void IAX2Transmit::ReportLists(PString & answer, bool getFullReport)
{
  PStringStream reply;
  PString aList;

  reply << "\n"
	<< PString("   SendNowFrames = ") << sendNowFrames.GetSize() << "\n";
  if (getFullReport) {
    sendNowFrames.ReportList(aList);
    reply << aList;
  }
  reply << PString("   AckingFrames  = ") << ackingFrames.GetSize() << "\n";
  if (getFullReport) {
    ackingFrames.ReportList(aList);
    reply << aList;
  }
  answer = reply;
}

void IAX2Transmit::ProcessSendList()
{
  for(;;) {
    IAX2Frame * active = sendNowFrames.GetLastFrame();
    if (active == NULL) 
      break;
    
    PBoolean isFullFrame = PFalse;
    if (PIsDescendant(active, IAX2FullFrame)) {
      isFullFrame = PTrue;
      IAX2FullFrame *f= (IAX2FullFrame *)active;
      if (f->DeleteFrameNow()) {
	PTRACE(6, "This frame has timed out, so do not transmit" <<  f->IdString());
	delete f;
	continue;
      }
    }
    
    if (!active->TransmitPacket(sock)) {
      PTRACE(4, "Delete  " << active->IdString() << " as transmit failed.");
      delete active;
      continue;
    }
    
    if (!isFullFrame) {
      PTRACE(4, "Delete this frame as it is a mini frame, and continue" << active->IdString());
      delete active;
      continue;
    }
    
    IAX2FullFrame *f= (IAX2FullFrame *)active;
    if (f->IsAckFrame() || f->IsVnakFrame()) {
      delete f;
      continue;
    }
    
    if (!active->CanRetransmitFrame()) {
      delete f;
      continue;
    }
    
    PTRACE(5, "Add frame " << *active << " to list of frames waiting on acks");
    ackingFrames.AddNewFrame(active);
  }
}

/* The comment below is magic for those who use emacs to edit this file. */
/* With the comment below, the tab key does auto indent to 2 spaces.     */

/*
 * Local Variables:
 * mode:c
 * c-basic-offset:2
 * End:
 */
